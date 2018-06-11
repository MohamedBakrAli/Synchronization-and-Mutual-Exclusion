#include <pthread.h>
#include "caltrain.h"

void station_init(struct station *station)
{
	station->waiting_train_passangers = 0;
	station->on_board_passangers = 0;
	pthread_mutex_init(&station->train_lock, NULL);
	pthread_cond_init(&station->arrive_of_train, NULL);
	pthread_cond_init(&station->all_passangers_on_board, NULL);
}

void station_load_train(struct station *station, int count)
{
    // lock section as i have threads for trains don't modife in statio data in the same time.
    pthread_mutex_lock(&station->train_lock);
  	station->empty_seat_train = count;
	while (station->empty_seat_train > 0 && station->waiting_train_passangers > 0){
        pthread_cond_broadcast(&station->arrive_of_train);
        pthread_cond_wait(&station->all_passangers_on_board, &station->train_lock);
	}
	// initial the empty seats for the next train.
	station->empty_seat_train = 0;
	pthread_mutex_unlock(&station->train_lock);
}

void station_wait_for_train(struct station *station)
{
	// lock section as i have threads for passangers don't modife in statio data in the same time.
	pthread_mutex_lock(&station->train_lock);
    station->waiting_train_passangers ++;
    //passangers waiting for the next train as this trian is full.
    while(station->on_board_passangers == station->empty_seat_train)
        pthread_cond_wait(&station->arrive_of_train, &station->train_lock);
    station->on_board_passangers ++;
    station->waiting_train_passangers --;
	pthread_mutex_unlock(&station->train_lock);
}

void station_on_board(struct station *station)
{
    // lock section as i have threads for passangers don't modife in statio data in the same time.
    pthread_mutex_lock(&station->train_lock);
    station->on_board_passangers --;
    station->empty_seat_train --;
    //train waiting till there is no empty seat in or all passangers seated and there is no waiting passanger.
    if (station->empty_seat_train == 0 ||(station->on_board_passangers == 0 &&
        station->waiting_train_passangers == 0))
        pthread_cond_signal(&station->all_passangers_on_board);
    pthread_mutex_unlock(&station->train_lock);
}
