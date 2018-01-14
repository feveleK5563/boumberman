#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//キャラクタ汎用クラス
//-------------------------------------------------------------------
#include "GameEngine_Ver3_7.h"
#include "ImageManager.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数

	//汎用状態管理
	enum State
	{
		Non,		//描画も更新もしない
		State1,		//状態1
		State2,		//状態2
		State3,		//状態3
		Death,		//死亡
	};
	State state;

	//キャラクターの動作
	ML::Vec2	pos;		//キャラクターの座標
	ML::Vec2	speed;		//キャラクターのスピード
	ML::Box2D	hitBase;	//当たり判定範囲

	//左右上下の向き
	enum AngleLRUD { Left, Right, Up, Down };
	AngleLRUD angleLRUD;

	//その他
	ImageManager image;
	int	 cntTime;	//汎用タイマー

	//メンバ変数の初期化
	BChara():
		pos(0, 0),
		speed(0, 0),
		hitBase(0, 0, 0, 0),
		angleLRUD(Left),
		image(ImageManager::ImageManager()),
		cntTime(0){}

	virtual ~BChara(){}

	//--------------------------------------------------------
	//以下キャラクタ共通メソッド

	void NomalMove();
	void CheckHitMove();
};
