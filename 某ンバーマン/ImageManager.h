#pragma once
#include "MyPG.h"

//画像の分割管理とアニメーション
class ImageManager
{
public:

	vector<ML::Box2D*>	charaChip;		//キャラクタの素材
	int					defImageNum;	//差分用画像番号
	int					baseImageNum;	//基準画像番号
	POINT				drawPos;		//描画の基準座標
	float				animCnt;		//アニメーションカウンタ
	bool				animTurn;		//アニメーションが反転しているか否か

	ImageManager() :
		defImageNum(0),
		baseImageNum(0),
		animCnt(0),
		animTurn(false)
	{
		drawPos.x = 0;
		drawPos.y = 0;
	};

	~ImageManager() {};

	//--------------------------------------------------------
	//以下使用メソッド

	void ImageCreate32x32(int, int, int, int);
	void ImageErase();
	void ImageRender(const ML::Vec2&, const string&, const ML::Color& = { 1, 1, 1, 1 });
};