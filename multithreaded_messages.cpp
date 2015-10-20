#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <pthread.h>

using namespace std;

struct thread_data {
  int  thread_id;
  string message;
  int char_per_message;
  int num_messages;
  int start_position;
};

pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;

void *print_message(void *threadarg)
{

  struct thread_data  *parameters = (struct thread_data *) threadarg;
  int thread_id = parameters->thread_id;
  string message = parameters->message;
  int char_per_message = parameters->char_per_message;
  int num_messages = parameters->num_messages;
  int N = message.length();
  message=message+message;
  int cur_start_position = parameters->start_position;
  for(int i=0;i<num_messages;i++) {
    string cur_message = message.substr(cur_start_position, char_per_message);
    pthread_mutex_lock( &cout_mutex );
    cout<<"Thread"<<thread_id<<": "<<cur_message<<endl;
    pthread_mutex_unlock( &cout_mutex );
    cur_start_position = (cur_start_position + char_per_message-1)%(N);
  }
  pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
  if (argc < 5) {
    cerr << "Usage: " << argv[0] << " STRING CHAR_COUNT THREAD_COUNT THREAD_ITERATIONS" << endl;
    return 1;
  }
  string message = argv[1];
  int char_count = atoi(argv[2]);
  int thread_count = atoi(argv[3]);
  int iterations_per_thread = atoi(argv[4]);
  vector<pthread_t> threads(thread_count);
  vector<struct thread_data> td(thread_count);

  int rc;
  pthread_attr_t attr;
  void *status;
//  pthread_attr_init(&attr);
//  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for(int i=0;i<thread_count;i++) {
    td[i].thread_id=i+1;
    td[i].message=message;
    td[i].char_per_message = char_count;
    td[i].num_messages=iterations_per_thread;
    td[i].start_position = (char_count*i)%(message.length());
    rc = pthread_create(&threads[i], NULL, print_message, (void *)&td[i]);
    if (rc){
      cout << "Error:unable to create thread," << rc << endl;
      exit(-1);
    }
  }
//  pthread_attr_destroy(&attr);
  for(int i=0; i < thread_count; i++ ){
    rc = pthread_join(threads[i], &status);
    if (rc){
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }
  }
  pthread_exit(NULL);
}
