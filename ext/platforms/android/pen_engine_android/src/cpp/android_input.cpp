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
#include "../../../../../../src/pen_engine.h"

#ifdef __PEN_ANDROID__
#include <android/asset_manager_jni.h>
#define ANDROID_MOBILE_ACCELERATION_CONSTANT 9.80665f

extern "C" {
    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeTouchesBegin(JNIEnv* env, jclass obj, jint id, jfloat x, jfloat y) {
        /*A touch has started*/
        pen::State::Get()->mobileMouse->push_back(new pen::Tap{ id, (double)x, (double)pen::State::Get()->actualScreenHeight - (double)y });
        pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::PRESSED, 0);
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeTouchesEnd(JNIEnv* env, jclass obj, jint id, jfloat x, jfloat y) {
        /*A touch has ended*/
        pen::State* inst = pen::State::Get();
        std::vector<pen::Tap*>* tempTaps = new std::vector<pen::Tap*>();
        for (int i = 0; i < inst->mobileMouse->size(); i++) {
            if (inst->mobileMouse->at(i)->id != id) {
                tempTaps->push_back(inst->mobileMouse->at(i));
            }
            else {
                /*Updates the mobileMouse vector with the released point for handling in mobile_click_callback before removing it*/
                inst->mobileMouse->at(i)->x = (double)x;
                inst->mobileMouse->at(i)->y = (double)inst->actualScreenHeight - (double)y;
            }
        }
        pen::Pen::mobile_click_callback(pen::in::KEYS::MOUSE_LEFT, pen::in::KEYS::RELEASED, 0);
        delete inst->mobileMouse;
        inst->mobileMouse = tempTaps;
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenSurfaceRenderer_nativeTouchesMove(JNIEnv* env, jclass obj, jintArray ids, jfloatArray xs, jfloatArray ys) {
        /*A touch is moving*/
        pen::State* inst = pen::State::Get();
        const int size = env->GetArrayLength(ids);
        jint* id = new jint[size];
        jfloat* x = new jfloat[size];
        jfloat* y = new jfloat[size];

        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < inst->mobileMouse->size(); j++) {
                if (inst->mobileMouse->at(j)->id == id[i]) {
                    inst->mobileMouse->at(j)->x = (double)x[i];
                    inst->mobileMouse->at(j)->y = (double)inst->actualScreenHeight - (double)y[i];
                    break;
                }
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_jamar_penengine_PenAccelerometer_onSensorChanged(JNIEnv* env, jclass obj, jfloat x, jfloat y, jfloat z, jlong timeStamp) {
        /*Handle tilt events*/
        double acelX = -((double)x / ANDROID_MOBILE_ACCELERATION_CONSTANT);
        double acelY = -((double)y / ANDROID_MOBILE_ACCELERATION_CONSTANT);
        double acelZ = -((double)z / ANDROID_MOBILE_ACCELERATION_CONSTANT);

        if (pen::State::Get()->mobileOnTiltCallback != nullptr) (*pen::State::Get()->mobileOnTiltCallback)(acelX, acelY, acelZ);
    }
}
#endif
