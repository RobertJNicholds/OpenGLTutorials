#include "CubeRobot.h"

Mesh* CubeRobot::cube = NULL;

CubeRobot::CubeRobot(void)
{
	SceneNode* body = new SceneNode(cube, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	body->SetModelScale(Vector3(10.0f, 15.0f, 5.0f));
	body->SetTransform(Matrix4::Translation(Vector3(0.0f, 35.0f, 0.0f)));
	AddChild(body);

	head = new SceneNode(cube, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	head->SetModelScale(Vector3(5.0f, 5.0f, 5.0f));
	head->SetTransform(Matrix4::Translation(Vector3(0.0f, 30.0f, 0.0f)));
	body->AddChild(head);

	leftArm = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	leftArm->SetModelScale(Vector3(3.0f, -18.0f, 3.0f));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-12.0f, 30.0f, -1.0f)));
	body->AddChild(leftArm);

	rightArm = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	rightArm->SetModelScale(Vector3(3.0f, -18.0f, 3.0f));
	rightArm->SetTransform(Matrix4::Translation(Vector3(12.0f, 30.0f, -1.0f)));
	body->AddChild(rightArm);

	SceneNode* leftLeg = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	leftLeg->SetModelScale(Vector3(3.0f, -17.5f, 3.0f));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8.0f, 0.0f, 0.0f)));
	body->AddChild(leftLeg);

	SceneNode* rightLeg = new SceneNode(cube, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	rightLeg->SetModelScale(Vector3(3.0f, -17.5f, 3.0f));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8.0f, 0.0f, 0.0f)));
	body->AddChild(rightLeg);

	body->SetBoundingRadius(15.0f);
	head->SetBoundingRadius(5.0f);

	leftArm->SetBoundingRadius(18.0f);
	rightArm->SetBoundingRadius(18.0f);

	leftLeg->SetBoundingRadius(18.0f);
	rightLeg->SetBoundingRadius(18.0f);
}

void CubeRobot::Update(float msec)
{
	transform = transform * Matrix4::Rotation(msec / 10.0f, Vector3(0.0f, 1.0f, 0.0f));

	head->SetTransform(head->GetTransform() *
		Matrix4::Rotation(-msec / 10.0f, Vector3(0.0f, 1.0f, 0.0f)));

	leftArm->SetTransform(leftArm->GetTransform() *
		Matrix4::Rotation(-msec / 10.0f, Vector3(1.0f, 0.0f, 0.0f)));

	rightArm->SetTransform(rightArm->GetTransform() *
		Matrix4::Rotation(-msec / 10.0f, Vector3(1.0f, 0.0f, 0.0f)));

	SceneNode::Update(msec);
}