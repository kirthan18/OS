#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>

int max_size = 0;
char* (*fetch_func)(char *url);
void (*edge_func)(char *from, char *to);


pthread_mutex_t link_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t page_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t link_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t link_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t page_empty = PTHREAD_COND_INITIALIZER;

typedef struct _links
{
	char *link;
}struct_links;

struct_links *links;

typedef struct _page
{
	char *page;
	struct _page *next_page; 
}page;

page *page_head = NULL;

int links_count = 0;
int page_count = 0;

int fill_links = 0;
int use_links = 0;

void put_links(char *link)
{
	links[fill_links].link = strdup(link);
	printf("Link to %s added at position %d in links queue\n", link, fill_links);
	fill_links = (fill_links + 1) % max_size; 
	links_count++;
	printf("Current links count in put_links: %d\n", links_count);
}

char* get_links()
{
	char *obtained_link = strdup(links[use_links].link);
	links_count--;
	printf("Link to %s obtained from position %d in links queue\n", obtained_link, use_links);
	printf("Current links count in get_links: %d\n", links_count);
	use_links = (use_links + 1) % max_size;
	return obtained_link;
}


void put_page(char *page_content)
{
	page *curr;

	page *new_page = (page*)malloc(sizeof(page));
	
	if(new_page != NULL)
	{
		new_page->page = strdup(page_content);
		new_page->next_page = NULL;
	

		if(page_head == NULL)
		{
			printf("Inserting at head of the page queue!\n");
			page_head = new_page;
		}else
		{
			curr = page_head;
			while(curr->next_page != NULL)
			{
				curr = curr->next_page;
			}
			curr->next_page = new_page;
		}
		page_count++;
		printf("Current page count in put_page: %d\n", page_count);
	}
}

char* get_page()
{
	char *obtained_page = NULL;
	page *temp;

	if(page_head != NULL)
	{
		obtained_page = strdup(page_head->page);
		temp = page_head;
		page_head = temp->next_page;
		free(temp);
		page_count--;
		printf("Current page count in get_page: %d\n", page_count);
	}
	return obtained_page;
}

void parse_page(char* page)
{	
	const char* delim = " \n";
	const char* link = "link";
	char* save;
	char* p;
	char *b;

	for (p = strtok_r(page, delim, &save); p; p = strtok_r(NULL, delim, &save))
	{
		//printf("Token = %s\n", p);
		if(strstr(p, link) != NULL)
		{
			b = p + 5;
			printf("Link points to : %s\n", b);
		}
	}
}

void *download_routine(void *args)
{
	char *link_to_download;
	char *fetched_page;

	pthread_mutex_lock(&link_mutex);
	while(links_count == 0)
	{
		pthread_cond_wait(&link_full, &link_mutex);
	}
	link_to_download = strdup(get_links());
	printf("Link obtained to download : %s\n", link_to_download);
	pthread_cond_signal(&link_empty);
	pthread_mutex_unlock(&link_mutex);

	fetched_page = fetch_func(link_to_download);

	//WHat to do here if several threads are trying to put downloaded data in page queue? is mutex enough?
	pthread_mutex_lock(&page_mutex);
	put_page(fetched_page);
	pthread_mutex_unlock(&page_mutex);
	return (void*)NULL;
}

void *parse_routine(void *args)
{
	char *page_to_parse;

	pthread_mutex_lock(&page_mutex);
	while(page_count == 0)
	{
		pthread_cond_wait(&page_empty, &page_mutex);
	}
	page_to_parse = get_page();
	pthread_mutex_unlock(&page_mutex);
	parse_page(page_to_parse);
	
	return (void*)NULL;
}

int crawl(char *start_url,
	  int download_workers,
	  int parse_workers,
	  int queue_size,
	  char * (*_fetch_fn)(char *url),
	  void (*_edge_fn)(char *from, char *to)) {

  int i = 0;
  int j = 0;
  int return_value = 0;

  fetch_func = _fetch_fn;

  char* page = _fetch_fn(start_url);
  assert(page != NULL);

  pthread_t *download_workers_t;
  pthread_t *parse_workers_t;


  download_workers_t = malloc(sizeof(pthread_t) * download_workers);
  parse_workers_t = malloc(sizeof(pthread_t) * parse_workers);

  max_size = queue_size;
  links = (struct_links*)malloc(queue_size * sizeof(struct_links));

  put_links(start_url);
  for(i = 0; i < download_workers; i++)
  {
  	return_value = pthread_create(&download_workers_t[i], NULL, download_routine, NULL);
  	printf("*****Creating download worker thread %d returned : %d*****\n", i, return_value);

  	if(return_value < 0)
  	{
  		printf("Error creating download_worker threads\n");
  	}
  }

  for(j = 0; j < parse_workers; j++)
  {
  	return_value = pthread_create(&parse_workers_t[j], NULL, parse_routine, NULL);
  	printf("*****Creating parse worker thread %d returned : %d*****\n", j, return_value);

  	if(return_value < 0)
  	{
  		printf("Error creating parse_worker threads\n");
  	}
  }


  //parse_page(page);
  //printf("\nPage : %s\n", page);
  free(page);
  return -1;
}
