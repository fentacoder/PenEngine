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
#include "camera.h"

namespace pen {
	Camera::Camera(){}

	Camera::Camera(pen::Vec3 viewPosition, int viewWidth, int viewHeight, float speed) {
		cameraPosition = viewPosition;
		screenWidth = viewWidth;
		screenHeight = viewHeight;
		cameraSpeed = speed;
		cameraFov = 90.0f;
		cameraZNear = 0.1f;
		cameraZFar = 1000.0f;
	}

	Camera::~Camera(){}

	void Camera::Update() {
		/*Update world view and projection matrices for 3D pixel items*/
		pen::State* inst = pen::State::Get();
		pen::State::Get()->pixel3DView = pen::op::Look(cameraPosition, cameraPosition + viewOrientation, at);
		pen::State::Get()->pixel3DProjection = pen::op::Perspective(cameraFov, (1280.0f / 720.0f), cameraZNear, cameraZFar);
	}

	void Camera::Update(float fov, float zNear, float zFar, pen::Mat4x4* view, pen::Mat4x4* proj, pen::Layer* layer) {
		/*Update world view and projection matrices*/
		pen::State* inst = pen::State::Get();
		cameraFov = fov;
		cameraZNear = zNear;
		cameraZFar = zFar;
		*view = pen::op::Look(cameraPosition, cameraPosition + viewOrientation, at);
		*proj = pen::op::Perspective(fov, ((float)inst->screenHeight / (float)inst->screenWidth), zNear, zFar);
		if (firstMove && !layer->is3D) {
			/*Set the initial position correctly for 2D objects*/
			pen::Mat4x4 mat = *view;
			mat.matrix[3][0] = 1.0f;
			mat.matrix[3][1] = 1.0f;
			*view = mat;
		}
	}

	void Camera::Update(float fov, float zNear, float zFar, pen::Mat4x4* view, pen::Mat4x4* proj) {
		/*Update world view and projection matrices for 3D pixel items*/
		pen::State* inst = pen::State::Get();
		cameraFov = fov;
		cameraZNear = zNear;
		cameraZFar = zFar;
		*view = pen::op::Look(cameraPosition, cameraPosition + viewOrientation, at);
		*proj = pen::op::Perspective(fov, (1280.0f / 720.0f), zNear, zFar);
	}

#ifndef __PEN_MOBILE__
	bool Camera::HandleInput(void* cameraWindow) {
		/*Orient the view based on user input*/
		bool isInput = false;
		if (!pen::State::Get()->handleCameraInput) return false;

		glfwwindow* window = (glfwwindow*)cameraWindow;
		if (glfwGetKey(window, pen::in::KEYS::UP) == pen::in::PRESSED
			|| glfwGetKey(window, pen::in::KEYS::UP) == pen::in::HELD) {
			/*Up*/
			cameraPosition += (at * cameraSpeed);
			firstMove = false;
			isInput = true;
		}

		if (glfwGetKey(window, pen::in::KEYS::DOWN) == pen::in::PRESSED
			|| glfwGetKey(window, pen::in::KEYS::DOWN) == pen::in::HELD) {
			/*Down*/
			cameraPosition += (at * -1.0f * cameraSpeed);
			firstMove = false;
			isInput = true;
		}

		if (glfwGetKey(window, pen::in::KEYS::W) == pen::in::PRESSED
			|| glfwGetKey(window, pen::in::KEYS::W) == pen::in::HELD) {
			/*Forward*/
			cameraPosition.z += (-1.0f * cameraSpeed);
			firstMove = false;
			isInput = true;
		}

		if (glfwGetKey(window, pen::in::KEYS::S) == pen::in::PRESSED
			|| glfwGetKey(window, pen::in::KEYS::S) == pen::in::HELD) {
			/*Backward*/
			cameraPosition.z += cameraSpeed;
			firstMove = false;
			isInput = true;
		}

		if (glfwGetKey(window, pen::in::KEYS::LEFT) == pen::in::PRESSED
			|| glfwGetKey(window, pen::in::KEYS::LEFT) == pen::in::HELD) {
			/*Left*/
			cameraPosition += ((pen::op::CrossProduct(viewOrientation, at).Normalize()) * -1.0f * cameraSpeed);
			firstMove = false;
			isInput = true;
		}

		if (glfwGetKey(window, pen::in::KEYS::RIGHT) == pen::in::PRESSED
			|| glfwGetKey(window, pen::in::KEYS::RIGHT) == pen::in::HELD) {
			/*Right*/
			cameraPosition += ((pen::op::CrossProduct(viewOrientation, at).Normalize()) * cameraSpeed);
			firstMove = false;
			isInput = true;
		}

		if (glfwGetMouseButton(window, pen::in::KEYS::MOUSE_LEFT) == pen::in::PRESSED
			|| glfwGetMouseButton(window, pen::in::KEYS::MOUSE_LEFT) == pen::in::HELD) {
			/*Change angle of view*/
			firstMove = false;
			glfwSetInputMode(window, glfw_CURSOR, glfw_CURSOR_HIDDEN);

			if (firstDrag) {
				glfwSetCursorPos(window, (screenWidth / 2), (screenHeight / 2));
				firstDrag = false;
			}

			double x = 0.0f, y = 0.0f;
			glfwGetCursorPos(window, &x, &y);

			float rotX = cameraSensitivity * (float)(y - (screenHeight / 2)) / screenHeight;
			float rotY = cameraSensitivity * (float)(x - (screenHeight / 2)) / screenHeight;

			pen::Vec3 newOrientation = pen::op::RotateVec(viewOrientation, -1.0f * rotX,
				(pen::op::CrossProduct(viewOrientation, at).Normalize()));

			if (!(pen::op::AngleBetween(newOrientation, at) <= 5.0f * 3.14159f / 180.0f
				|| pen::op::AngleBetween(newOrientation, at * -1.0f) <= 5.0f * 3.14159f / 180.0f)) {
				viewOrientation = newOrientation;
			}

			viewOrientation = pen::op::RotateVec(viewOrientation, -1.0f * rotY, at * -1.0f);

			/*Prevent mouse from escaping the screen*/
			glfwSetCursorPos(window, (screenWidth / 2), (screenHeight / 2));
			isInput = true;
		}
		else if (glfwGetMouseButton(window, pen::in::KEYS::MOUSE_LEFT) == pen::in::RELEASED) {
			/*Show mouse cursor*/
			glfwSetInputMode(window, glfw_CURSOR, glfw_CURSOR_NORMAL);
			firstDrag = true;
			isInput = true;
		}
		if (isInput && pen::State::Get()->onCameraEvent != nullptr) (*pen::State::Get()->onCameraEvent)();
		return isInput;
	}
#else
#if TARGET_OS_OSX
    bool Camera::HandleInput(int key, int action) {
        /*Orient the view based on user input for Mac*/
		bool isInput = false;
        if (!pen::State::Get()->handleCameraInput) return false;

        if (key == pen::in::KEYS::UP && action == pen::in::KEYS::PRESSED) {
            /*Up*/
            cameraPosition += (at * cameraSpeed);
            firstMove = false;
			isInput = true;
        }

        if (key == pen::in::KEYS::DOWN && action == pen::in::KEYS::PRESSED) {
            /*Down*/
            cameraPosition += (at * -1.0f * cameraSpeed);
            firstMove = false;
			isInput = true;
        }

        if (key == pen::in::KEYS::W && action == pen::in::KEYS::PRESSED) {
            /*Forward*/
			cameraPosition.z += (-1.0f * cameraSpeed);
            firstMove = false;
			isInput = true;
        }

        if (key == pen::in::KEYS::S && action == pen::in::KEYS::PRESSED) {
            /*Backward*/
			cameraPosition.z cameraSpeed;
            firstMove = false;
			isInput = true;
        }

        if (key == pen::in::KEYS::LEFT && action == pen::in::KEYS::PRESSED) {
            /*Left*/
            cameraPosition += ((pen::op::CrossProduct(viewOrientation, at).Normalize()) * -1.0f * cameraSpeed);
            firstMove = false;
			isInput = true;
        }

        if (key == pen::in::KEYS::RIGHT && action == pen::in::KEYS::PRESSED) {
            /*Right*/
            cameraPosition += ((pen::op::CrossProduct(viewOrientation, at).Normalize()) * cameraSpeed);
            firstMove = false;
			isInput = true;
        }

        if (key == pen::in::KEYS::SPACE && (action == pen::in::KEYS::PRESSED
            || action == pen::in::KEYS::HELD)) {
            /*Change angle of view*/
            firstMove = false;
			MapMacIOSMakeMouseHidden();

            if (firstDrag) {
                firstDrag = false;
            }

            double x = 0.0f, y = 0.0f;

            float rotX = cameraSensitivity * (float)(y - (screenHeight / 2)) / screenHeight;
            float rotY = cameraSensitivity * (float)(x - (screenHeight / 2)) / screenHeight;

            pen::Vec3 newOrientation = pen::op::RotateVec(viewOrientation, -1.0f * rotX,
                (pen::op::CrossProduct(viewOrientation, at).Normalize()));

            if (!(pen::op::AngleBetween(newOrientation, at) <= 5.0f * 3.14159f / 180.0f
                || pen::op::AngleBetween(newOrientation, at * -1.0f) <= 5.0f * 3.14159f / 180.0f)) {
                viewOrientation = newOrientation;
            }

            viewOrientation = pen::op::RotateVec(viewOrientation, -1.0f * rotY, at * -1.0f);
			isInput = true;
        }
        else if (key == pen::in::KEYS::SPACE && action == pen::in::KEYS::RELEASED) {
			MapMacIOSMakeMouseShow();
            firstDrag = true;
			isInput = true;
        }
		if (isInput && pen::State::Get()->onCameraEvent != nullptr) (*pen::State::Get()->onCameraEvent)();
        return isInput;
    }
#endif
#endif
}
