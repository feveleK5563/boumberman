//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Bomb.h"
#include  "Task_Player.h"
#include  "Task_Stage.h"

namespace  Bomb
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[Xฬ๚ป
	bool  Resource::Initialize()
	{
		imageName = "Bomb";
		DG::Image_Create(imageName, "./data/image/Bomb.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[Xฬ๐๚
	bool  Resource::Finalize()
	{
		DG::Image_Erase(imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//u๚ปv^XNถฌษP๑พฏsค
	bool  Object::Initialize()
	{
		//X[p[NX๚ป
		__super::Initialize(defGroupName, defName, true);
		//\[XNXถฌor\[XคL
		this->res = Resource::Create();

		//f[^๚ป
		render2D_Priority[1] = 0.5f;

		bombMapPos = { 0, 0 };
		state = State1;
		
		image.ImageCreate32x32(0, 0, 3, 1);
		image.ImageCreate32x32(0, 1, 7, 4);

		//^XNฬถฌ

		return  true;
	}
	//-------------------------------------------------------------------
	//uIนv^XNมลษP๑พฏsค
	bool  Object::Finalize()
	{
		//f[^^XN๐๚
		image.ImageErase();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//๘ซpฌ^XNฬถฌ
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[ษsค
	void  Object::UpDate()
	{
		switch (state)
		{
		case State1:
			NoBomb();
			break;

		case Death:
			ActiveBomb();
			break;
		}
	}
	//-------------------------------------------------------------------
	//uQc`ๆvPt[ษsค
	void  Object::Render2D_AF()
	{
		image.ImageCenterRender(pos, res->imageName);
	}

	//-------------------------------------------------------------------
	//ขญฬXV
	void Object::NoBomb()
	{
		if (cntTime >= 135)
		{
			cntTime = 0;
			StartBomb();
		}
		else
		{
			image.animCnt = float(noBombTable[(cntTime / 15) % 4]);
			++cntTime;

			if (auto stage = ge->GetTask_One_GN<Stage::Object>("Xe[W", ""))
			{
				stage->mapData[bombMapPos.y][bombMapPos.x];
			}

		}
	}

	//-------------------------------------------------------------------
	//ญฬJn
	void Object::StartBomb()
	{
		image.baseImageNum = 3;
		state = Death;

		auto player = ge->GetTask_One_GN<Player::Object>("{า", "vC[");
		auto stage = ge->GetTask_One_GN<Stage::Object>("Xe[W", "");
		if (player == nullptr || stage == nullptr)
			Kill();

		stage->mapData[bombMapPos.y][bombMapPos.x] = -3;

		for (int i = 0; i < 4; ++i)
		{
			int fireLength = player->bombPower, setBaseImage;
			POINT nextMapPos = bombMapPos;
			LONG *XorY, addOrSub;
			switch (i)
			{
			case 0:	//ถ
				XorY = &nextMapPos.x;
				addOrSub = -1;
				setBaseImage = 8;
				break;

			case 1: //E
				XorY = &nextMapPos.x;
				addOrSub = 1;
				setBaseImage = 8;
				break;

			case 2: //ใ
				XorY = &nextMapPos.y;
				addOrSub = -1;
				setBaseImage = 9;
				break;

			case 3: //บ
				XorY = &nextMapPos.y;
				addOrSub = 1;
				setBaseImage = 9;
				break;
			}

			*XorY += addOrSub;

			while (stage->mapData[nextMapPos.y][nextMapPos.x] == -1)
			{
				stage->mapData[nextMapPos.y][nextMapPos.x] = -3;
				auto newbomb = Bomb::Object::Create(true);
				newbomb->state = Death;
				newbomb->image.baseImageNum = setBaseImage;
				newbomb->pos = { float(nextMapPos.x * 32 + 16), float(nextMapPos.y * 32 + 16) };
				newbomb->bombMapPos = nextMapPos;

				*XorY += addOrSub;
				--fireLength;

				if (fireLength <= 0)
				{
					newbomb->image.baseImageNum = 4 + i;
					break;
				}
			}
		}
	}

	//-------------------------------------------------------------------
	//ญฬXV
	void Object::ActiveBomb()
	{
		if (cntTime >= 28)
		{
			DeleteBomb();
		}
		else
		{
			image.animCnt = float(acBombTable[(cntTime / 4) % 7]);
			++cntTime;
		}
	}

	//-------------------------------------------------------------------
	//eฬมล
	void Object::DeleteBomb()
	{
		if (image.baseImageNum == 3)
		{
			if (auto player = ge->GetTask_One_GN<Player::Object>("{า", "vC["))
				--player->bombNum;
		}

		if (auto stage = ge->GetTask_One_GN<Stage::Object>("Xe[W", ""))
			stage->mapData[bombMapPos.y][bombMapPos.x] = -1;

		Kill();
	}

	//
	//ศบอ๎{IษฯXsvศ\bh
	//
	//-------------------------------------------------------------------
	//^XNถฌ๛
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWษo^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYษธsตฝ็Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//\[XNXฬถฌ
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}