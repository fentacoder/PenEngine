/*************************************************************************************************
 Copyright 2021 Jamar Phillip

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*************************************************************************************************/
#pragma once
#include "../../../src/state/config.h"

#ifdef __PEN_IOS__
/*
  This file is for the instantiation of Pen Engine by the user in order to include it's OnCreate function.
  In the OnCreate function it is expected that pen::Pen::SetMobileCallbacks(); is called.
*/
#import "../../../../app.h"
#import "ios_view_delegate.h"
#import "ios_vertex_buffer.h"
#import "ios_argument_buffer.h"
#import "ios_index_buffer.h"
#import "ios_shader.h"

@interface PenIOSAppDelegate : NSObject

    /*These methods are inherited from NSObject*/
    - (void) applicationWillFinishLaunching: (NSNotification*) pNotification;
    - (void) applicationDidFinishLaunching: (NSNotification*) pNotification;
    - (void) applicationDidBecomeActive: (NSNotification*) pNotification;
    - (void) applicationWillResignActive: (NSNotification*) pNotification;
#ifndef TARGET_OS_IOS
    - (bool) applicationShouldTerminateAfterLastWindowClosed: (NSApplication*) pSender;
#endif
@end
#endif
