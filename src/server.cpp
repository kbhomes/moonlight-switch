///*
// * This file is part of Moonlight Embedded.
// *
// * Copyright (C) 2015-2017 Iwan Timmer
// *
// * Moonlight is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation; either version 3 of the License, or
// * (at your option) any later version.
// *
// * Moonlight is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with Moonlight; if not, see <http://www.gnu.org/licenses/>.
// */

#include "server.h"

#include <stdio.h>
#include <functional>

#define MOONLIGHT_DATA_DIR "sdmc:/switch/moonlight-switch/"

Server::Server(PCONFIGURATION config)
    : config_(config),
      opened_(false)
{
    threadCreate(
        &worker_thread_,
        [](void *context) { static_cast<Server *>(context)->thread_(); },
        this,
        0x10000,
        0x2C,
        -2
    );

    ueventCreate(&event_apps_, true);
    ueventCreate(&event_start_stream_, true);
    ueventCreate(&event_stop_stream_, true);
    ueventCreate(&event_close_, true);
}

Server::~Server() {
    threadClose(&worker_thread_);
}

void Server::open() {
    threadStart(&worker_thread_);
    opened_ = true;
}

void Server::close() {
    ueventSignal(&event_close_);
    threadWaitForExit(&worker_thread_);
    opened_ = false;
}

bool Server::opened() {
    return opened_;
}

bool Server::paired() {
//    return server_.paired;
    return false;
}

future<PAPP_LIST> Server::apps()
{
    // Tell the worker to fetch the app list, and return a future
    ueventSignal(&event_apps_);
    return apps_promise_.get_future();
}

void Server::startStream()
{

}

void Server::stopStream()
{

}

CONNECTION_LISTENER_CALLBACKS Server::getCallbacks()
{
    return {0};
}

void Server::thread_()
{
//    int ret = gs_init(&server_, config_->address, MOONLIGHT_DATA_DIR "key", config_->debug_level, config_->unsupported);
//    if (ret == GS_OUT_OF_MEMORY) {
//        fprintf(stderr, "Not enough memory\n");
//    }
//    else if (ret == GS_ERROR) {
//        fprintf(stderr, "GameStream error: %s\n", gs_error);
//    }
//    else if (ret == GS_INVALID) {
//        fprintf(stderr, "Invalid data received from server: %s\n", gs_error);
//    }
//    else if (ret == GS_UNSUPPORTED_VERSION) {
//        fprintf(stderr, "Unsupported version: %s\n", gs_error);
//    }
//    else if (ret != GS_OK) {
//        fprintf(stderr, "Can't connect to server %s, error: %s\n", config_->address, gs_error);
//    }

    Result rc;
    int index = -1;

    while (1) {
        rc = waitMulti(&index, -1,
                       waiterForUEvent(&event_apps_),
                       waiterForUEvent(&event_start_stream_),
                       waiterForUEvent(&event_stop_stream_),
                       waiterForUEvent(&event_close_));

        if (R_SUCCEEDED(rc)) {
            switch (index) {
            case 0: threadApps_(); break;
            case 1: threadStartStream_(); break;
            case 2: threadStopStream_(); break;
            }

            if (index == 3) {
                // Close connection
                break;
            }
        }
    };
}

void Server::threadApps_() {
    // Get the apps list from the streaming server
//    int rc = gs_applist(&server_, &apps_);

//    if (rc != GS_OK) {
//      apps_ = nullptr;
//    }

    svcSleepThread(3000000000ull);

    // Update the promise state
    apps_promise_.resolve("Sample apps string");
}

void Server::threadStartStream_() {}
void Server::threadStopStream_() {}


////pthread_t main_thread_id = 0;
////bool connection_debug;

////int pair_check(PSERVER_DATA server) {
////  if (!server->paired) {
////    fprintf(stderr, "You must pair with the PC first\n");
////    return 0;
////  }

////  return 1;
////}

////size_t get_app_list(PSERVER_DATA server, PAPP_LIST *list) {
////  if (gs_applist(server, list) != GS_OK) {
////    fprintf(stderr, "Can't get app list\n");
////    *list = NULL;
////    return 0;
////  }

////  PAPP_LIST curr = *list;
////  size_t count = 0;
////  while (curr) {
////    count++;
////    curr = curr->next;
////  }

////  return count;
////}

////int get_app_id(PSERVER_DATA server, const char *name) {
////  PAPP_LIST list = NULL;
////  if (gs_applist(server, &list) != GS_OK) {
////    fprintf(stderr, "Can't get app list\n");
////    return -1;
////  }

////  while (list != NULL) {
////    if (strcmp(list->name, name) == 0)
////      return list->id;

////    list = list->next;
////  }
////  return -1;
////}

////int stream_start(PSERVER_DATA server, PCONFIGURATION config, int appId, enum platform system) {
////  int gamepads = 0;
////  int gamepad_mask = 0;
////  for (int i = 0; i < gamepads && i < 4; i++)
////    gamepad_mask = (gamepad_mask << 1) + 1;

////  int ret = gs_start_app(server, &config->stream, appId, config->sops, config->localaudio, gamepad_mask);
////  if (ret < 0) {
////    if (ret == GS_NOT_SUPPORTED_4K)
////      fprintf(stderr, "Server doesn't support 4K\n");
////    else if (ret == GS_NOT_SUPPORTED_MODE)
////      fprintf(stderr, "Server doesn't support %dx%d (%d fps) or try --unsupported option\n", config->stream.width, config->stream.height, config->stream.fps);
////    else if (ret == GS_ERROR)
////      fprintf(stderr, "Gamestream error: %s\n", gs_error);
////    else
////      fprintf(stderr, "Errorcode starting app: %d\n", ret);

////    return -1;
////  }

////  int drFlags = 0;
////  if (config->fullscreen)
////    drFlags |= DISPLAY_FULLSCREEN;

////  if (config->debug_level > 0) {
////    printf("Stream %d x %d, %d fps, %d kbps\n", config->stream.width, config->stream.height, config->stream.fps, config->stream.bitrate);
////    connection_debug = true;
////  }

////  platform_start(system);
////  LiStartConnection(&server->serverInfo, &config->stream, &connection_callbacks, platform_get_video(system), platform_get_audio(system, config->audio_device), NULL, drFlags, config->audio_device, 0);

////  return 0;
////}

////int stream_stop(enum platform system) {
////  LiStopConnection();
////  platform_stop(system);

////  return 0;
////}

////// Moonlight connection callbacks
////static void connection_stage_starting(int stage) {}
////static void connection_stage_complete(int stage) {}
////static void connection_stage_failed(int stage, long errorCode) {}
////static void connection_started(void) {
////  printf("[*] Connection started\n");
////}
////static void connection_terminated(long error) {
////  perror("[*] Connection terminated");
////}
////static void connection_display_message(const char *msg) {
////  printf("[*] %s\n", msg);
////}
////static void connection_display_transient_message(const char *msg) {
////  printf("[*] %s\n", msg);
////}
////static void connection_log_message(const char* format, ...) {
////  va_list arglist;
////  va_start(arglist, format);
////  vprintf(format, arglist);
////  va_end(arglist);
////}

////CONNECTION_LISTENER_CALLBACKS connection_callbacks = {
////  .stageStarting = connection_stage_starting,
////  .stageComplete = connection_stage_complete,
////  .stageFailed = connection_stage_failed,
////  .connectionStarted = connection_started,
////  .connectionTerminated = connection_terminated,
////  .displayMessage = connection_display_message,
////  .displayTransientMessage = connection_display_transient_message,
////  .logMessage = connection_log_message,
////};
