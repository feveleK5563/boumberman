#include "ImageManager.h"

//-------------------------------------------------------------------
//�摜�̕���
//�����F�ǂݍ��݊J�n�ʒuXY(int, int), �摜�̕�����WH(int, int)
void ImageManager::ImageCreate32x32(int startX, int startY, int width, int height)
{
	for (int y = startY; y < startY + height; ++y)
	{
		for (int x = startX; x < startX + width; ++x)
		{
			charaChip.emplace_back(new ML::Box2D(x * 32, y * 32, 32, 32));
		}
	}
}

//-------------------------------------------------------------------
//�摜�̕�������
void ImageManager::ImageErase()
{
	int size = charaChip.size();
	for (int i = 0; i < size; ++i)
		delete charaChip[i];
	charaChip.clear();
	charaChip.shrink_to_fit();
}

//-------------------------------------------------------------------
//�摜�̕`��
//�����F�\�����W(ML::Vec2), �摜��(string), �\���F(ML::Color �f�t�H���g��(1, 1, 1, 1))
void ImageManager::ImageRender(const ML::Vec2& pos, const string& imageName, const ML::Color& color)
{
	if (baseImageNum < 0)
		return;
	ML::Box2D draw = { -drawPos.x, -drawPos.y, 32, 32 };
	draw.Offset(pos);
	ML::Box2D src = *charaChip[defImageNum + baseImageNum + int(animCnt)];
	if (animTurn)
	{
		src.x += src.w;
		src.w *= -1;
	}
	DG::Image_Draw(imageName, draw, src, color);
}