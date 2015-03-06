#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include "GameGlobals.h"
#ifdef ICEMAN_DEBUG_MODE
	#include "GameWorld.h"
#endif

class GameWorld;

class GameObject : public CCSprite
{
public:
	GameObject() : game_world_(NULL),
		type_(E_GAME_OBJECT_NONE), 
		aabb_(CCRectZero), 

#ifdef ICEMAN_DEBUG_MODE
		aabb_node_(NULL),
#endif

		speed_(CCPointZero) {}

	virtual ~GameObject()
	{}

	virtual void SetAABB(CCRect aabb)
	{
		aabb_ = aabb;

#ifdef ICEMAN_DEBUG_MODE
		// draw the AABB in debug mode only
		CCPoint vertices[4];
		vertices[0] = CCPointZero;
		vertices[1] = ccp(0, aabb_.size.height);
		vertices[2] = ccp(aabb_.size.width, aabb_.size.height);
		vertices[3] = ccp(aabb_.size.width, 0);
		
		aabb_node_ = CCDrawNode::create();
		aabb_node_->drawPolygon(vertices, 4, ccc4f(0, 0, 0, 0), 1, ccc4f(1, 0, 0, 1));
		aabb_node_->setPosition(aabb_.origin);
		game_world_->addChild(aabb_node_);
#endif
	}

	inline CCRect GetAABB() { return aabb_; }
	virtual void SetSpeed(CCPoint speed) { speed_ = speed; }
	inline CCPoint GetSpeed() { return speed_; }

	virtual void Update()
	{
		aabb_.origin.x += speed_.x;
		aabb_.origin.y += speed_.y;

#ifdef ICEMAN_DEBUG_MODE
		aabb_node_->setPosition(getParent()->convertToWorldSpace(aabb_.origin));
#endif
	}
	virtual void CollisionResponse(int tile_col, int tile_row, ECollisionType collision_type) {}

protected:
	GameWorld* game_world_;
	EGameObjectType type_;
	CCRect aabb_;
	CCPoint speed_;

#ifdef ICEMAN_DEBUG_MODE
public:
	CCDrawNode* aabb_node_;
#endif
};

#endif // GAME_OBJECT_H_
