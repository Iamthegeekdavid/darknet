/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Portions created by Alan Antonuk are Copyright (c) 2012-2013
 * Alan Antonuk. All Rights Reserved.
 *
 * Portions created by VMware are Copyright (c) 2007-2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Portions created by Tony Garnock-Jones are Copyright (c) 2009-2010
 * VMware, Inc. and Tony Garnock-Jones. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include "utils.h"

#include <time.h>

//int main(int argc, char const *const *argv) {
 int sendmessage(char * objname, int objprecision) {
	char const *hostname;
	int port, status;
	char const *exchange;
	char const *routingkey;
	char const *messagebody;
  time_t cur_time;

  printf("entered sendmessage function");

	hostname = "localhost"; //URL
	//hostname = "amqp://svxvaiup:e-fEFcGJZgm2vKk1D7OykIf2GHTBI16z@eagle.rmq.cloudamqp.com/svxvaiup";
	port = 5672; //port
	//exchange = "amq.direct"; // use empty string
	exchange = "";
	routingkey = "darknet";  //Queue name

	//send Rabbitmq message in Json format
	//messagebody = "{\"detected object\": 98}";
	char* part1 = "{\"";
	char* part2 = "text";
	//char* part2 = objname;
	char* part3 = "\": ";
	char* part4[256];
	strcpy(part4, "\"detected: ");
	strcat(part4, objname);
	strcat(part4, "\"");
	//char* part4[10];
	//_itoa(objprecision, part4, 10);
	char* part5 = "}";

	messagebody = malloc(strlen(part1) + strlen(part2) + strlen(part3) + strlen(part4) + strlen(part5));
	strcpy(messagebody, part1);
	strcat(messagebody, part2);
	strcat(messagebody, part3);
	strcat(messagebody, part4);
	strcat(messagebody, part5);
	printf("messagebody = %s\n", messagebody);
  //time(&cur_time);
  //printf("current time = %d\n", (int)cur_time);

  //hostname = argv[1]; //real case: URL, curious why not need user/password for verification
  //port = atoi(argv[2]); 
  //exchange = argv[3]; //amq.direct: what does it mean?
  //routingkey = argv[4]; //test: what does it mean?
  //messagebody = argv[5]; //messagebody: to write in JSON format

  amqp_socket_t *socket = NULL;
  amqp_connection_state_t conn;
  conn = amqp_new_connection();

  socket = amqp_tcp_socket_new(conn);
  if (!socket) {
    die("creating TCP socket");
  }

  status = amqp_socket_open(socket, hostname, port);
  if (status) {
    die("opening TCP socket");
  }

  die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                               "guest", "guest"),
                    "Logging in");
  amqp_channel_open(conn, 1);
  die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

  {
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2; /* persistent delivery mode */
    die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange),
                                    amqp_cstring_bytes(routingkey), 0, 0,
                                    &props, amqp_cstring_bytes(messagebody)),
                 "Publishing");
  }

  die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                    "Closing channel");
  die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                    "Closing connection");
  die_on_error(amqp_destroy_connection(conn), "Ending connection");
  return 0;
}

 int sendmessage_v2(char* imagepath, char * objname, int objprecision) {
	char const *hostname;
	int port, status;
	char const *exchange;
	char const *routingkey;
	char const *messagebody;
  time_t cur_time;

  printf("entered sendmessage function");

	hostname = "localhost"; //URL
	//hostname = "amqp://svxvaiup:e-fEFcGJZgm2vKk1D7OykIf2GHTBI16z@eagle.rmq.cloudamqp.com/svxvaiup";
	port = 5672; //port
	//exchange = "amq.direct"; // use empty string
	exchange = "";
	routingkey = "darknet";  //Queue name

	//send Rabbitmq message in Json format
	//messagebody = "{\"detected object\": 98}";
	char* part1 = "{\"";
	char* part2 = "text";
	//char* part2 = objname;
	char* part3 = "\": ";
	char* part4[50];
	strcpy(part4, "\"detected: ");
	strcat(part4, objname);
	strcat(part4, "\"");
  char* part5[256];
  strcpy(part5, ",\"image\":\"");
  strcat(part5, imagepath);
  strcat(part5, "\"");
	char* part6 = "}";

	messagebody = malloc(strlen(part1) + strlen(part2) + strlen(part3) + strlen(part4) + strlen(part5) + strlen(part6));
	strcpy(messagebody, part1);
	strcat(messagebody, part2);
	strcat(messagebody, part3);
	strcat(messagebody, part4);
	strcat(messagebody, part5);
  strcat(messagebody, part6);
	printf("messagebody = %s\n", messagebody);
  //time(&cur_time);
  //printf("current time = %d\n", (int)cur_time);

  //hostname = argv[1]; //real case: URL, curious why not need user/password for verification
  //port = atoi(argv[2]); 
  //exchange = argv[3]; //amq.direct: what does it mean?
  //routingkey = argv[4]; //test: what does it mean?
  //messagebody = argv[5]; //messagebody: to write in JSON format

  amqp_socket_t *socket = NULL;
  amqp_connection_state_t conn;
  conn = amqp_new_connection();

  socket = amqp_tcp_socket_new(conn);
  if (!socket) {
    die("creating TCP socket");
  }

  status = amqp_socket_open(socket, hostname, port);
  if (status) {
    die("opening TCP socket");
  }

  die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                               "guest", "guest"),
                    "Logging in");
  amqp_channel_open(conn, 1);
  die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

  {
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2; /* persistent delivery mode */
    die_on_error(amqp_basic_publish(conn, 1, amqp_cstring_bytes(exchange),
                                    amqp_cstring_bytes(routingkey), 0, 0,
                                    &props, amqp_cstring_bytes(messagebody)),
                 "Publishing");
  }

  die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
                    "Closing channel");
  die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
                    "Closing connection");
  die_on_error(amqp_destroy_connection(conn), "Ending connection");
  return 0;
}
