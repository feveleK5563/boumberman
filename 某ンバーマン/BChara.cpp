//-------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "BChara.h"
#include "MyPG.h"
#include "Task_Stage.h"

//-------------------------------------------------------------------
//�ʏ�ړ�
void BChara::NomalMove()
{
	pos += speed;
}

//-------------------------------------------------------------------
//�����蔻��t���̈ړ�
void BChara::CheckHitMove()
{
	ML::Vec2 est = speed;

	for (int i = 0; i < 2; ++i)
	{
		auto stage = ge->GetTask_One_GN<Stage::Object>("�X�e�[�W", "����");
		if (stage == nullptr)
			return;

		float* cpos;
		float* cspd;
		switch (i)
		{
		case 0:	//X
			cpos = &pos.x;
			cspd = &est.x;
			break;

		case 1:	//Y
			cpos = &pos.y;
			cspd = &est.y;
			break;

		default:
			return;
		}

		while (*cspd != 0.f)
		{
			float pre = *cpos;
			if		(*cspd > 1.f)	{ ++*cpos; --*cspd; }
			else if (*cspd < -1.f)	{ --*cpos; ++*cspd; }
			else					{ *cpos += *cspd; *cspd = 0.f; }
			ML::Box2D hb = hitBase.OffsetCopy(pos);
			if (stage->MapHitCheck(hb))
			{
				*cpos = pre;
				break;
			}
		}
	}
}