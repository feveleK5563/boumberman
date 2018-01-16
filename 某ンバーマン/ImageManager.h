#pragma once
#include "MyPG.h"

//�摜�̕����Ǘ��ƃA�j���[�V����
class ImageManager
{
public:

	vector<ML::Box2D*>	charaChip;		//�L�����N�^�̑f��
	int					defImageNum;	//�����p�摜�ԍ�
	int					baseImageNum;	//��摜�ԍ�
	POINT				drawPos;		//�`��̊���W
	float				animCnt;		//�A�j���[�V�����J�E���^
	bool				animTurn;		//�A�j���[�V���������]���Ă��邩�ۂ�

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
	//�ȉ��g�p���\�b�h

	void ImageCreate32x32(int, int, int, int);
	void ImageErase();
	void ImageRender(const ML::Vec2&, const string&, const ML::Color& = { 1, 1, 1, 1 });
};