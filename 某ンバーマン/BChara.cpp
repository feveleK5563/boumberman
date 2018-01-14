//-------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-------------------------------------------------------------------
#include "BChara.h"
#include "MyPG.h"
#include "Task_Stage.h"

//-------------------------------------------------------------------
//通常移動
void BChara::NomalMove()
{
	pos += speed;
}

//-------------------------------------------------------------------
//当たり判定付きの移動
void BChara::CheckHitMove()
{
	ML::Vec2 est = speed;

	for (int i = 0; i < 2; ++i)
	{
		auto stage = ge->GetTask_One_GN<Stage::Object>("ステージ", "統括");
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