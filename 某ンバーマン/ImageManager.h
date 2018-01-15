#pragma once
#include "MyPG.h"

//�摜�̕����Ǘ��ƃA�j���[�V����
class ImageManager
{
public:

	vector<ML::Box2D*>	charaChip;		//�L�����N�^�̑f��
	int					baseImageNum;	//��摜�ԍ�
	ML::Box2D			draw;
	float				animCnt;		//�A�j���[�V�����J�E���^
	bool				animTurn;		//�A�j���[�V���������]���Ă��邩�ۂ�

	ImageManager() :
		baseImageNum(0),
		draw(0, 0, 32, 32),
		animCnt(0),
		animTurn(false) {};

	~ImageManager() {};

	//--------------------------------------------------------
	//�ȉ��g�p���\�b�h

	void ImageCreate32x32(int, int, int, int);
	void ImageErase();
	void ImageRender(const ML::Vec2&, const string&, const ML::Color& = { 1, 1, 1, 1 });
};