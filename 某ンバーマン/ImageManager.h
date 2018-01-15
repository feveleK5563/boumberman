#pragma once
#include "MyPG.h"

//画像の分割管理とアニメーション
class ImageManager
{
public:

	vector<ML::Box2D*>	charaChip;		//キャラクタの素材
	int					baseImageNum;	//基準画像番号
	ML::Box2D			draw;
	float				animCnt;		//アニメーションカウンタ
	bool				animTurn;		//アニメーションが反転しているか否か

	ImageManager() :
		baseImageNum(0),
		draw(0, 0, 32, 32),
		animCnt(0),
		animTurn(false) {};

	~ImageManager() {};

	//--------------------------------------------------------
	//以下使用メソッド

	void ImageCreate32x32(int, int, int, int);
	void ImageErase();
	void ImageRender(const ML::Vec2&, const string&, const ML::Color& = { 1, 1, 1, 1 });
};