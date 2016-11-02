#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour)
{
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	modelScale = Vector3(1.0f, 1.0f, 1.0f);

	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
}

SceneNode::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode *s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChildren()
{
	if (parent == NULL)
	{
		for (unsigned int i = 0; i < children.size(); ++i)
		{
			delete children[i];
		}
	}
}


void SceneNode::Draw()
{
	if (mesh) { mesh->Draw(); }
}

void SceneNode::Update(float msec)
{
	if (parent) //Move this obj by parents transform
		worldTransform = parent->worldTransform * transform;

	else //Root Node
		worldTransform = transform;

	for (std::vector<SceneNode*>::iterator i = children.begin();
										   i != children.end(); ++i)
	{
		(*i)->Update(msec);
	}
}