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
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Bomb";
		DG::Image_Create(imageName, "./data/image/Bomb.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		DG::Image_Erase(imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		render2D_Priority[1] = 0.5f;

		bombMapPos = { 0, 0 };
		state = State1;
		
		image.ImageCreate32x32(0, 0, 3, 1);
		image.ImageCreate32x32(0, 1, 7, 4);
		image.draw = { -16, -16, 32, 32 };

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		image.ImageErase();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
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
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		image.ImageRender(pos, res->imageName);
	}

	//-------------------------------------------------------------------
	//未爆発時の更新
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

			//誘爆
			if (auto stage = ge->GetTask_One_GN<Stage::Object>("ステージ", "統括"))
			{
				if (stage->mapData[bombMapPos.y][bombMapPos.x] == stage->CausingEx)
					cntTime = 135;

			}

		}
	}

	//-------------------------------------------------------------------
	//爆発の開始
	void Object::StartBomb()
	{
		image.baseImageNum = 3;
		state = Death;

		auto player = ge->GetTask_One_GN<Player::Object>("本編", "プレイヤー");
		auto stage = ge->GetTask_One_GN<Stage::Object>("ステージ", "統括");
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
			case 0:	//左
				XorY = &nextMapPos.x;
				addOrSub = -1;
				setBaseImage = 8;
				break;

			case 1: //右
				XorY = &nextMapPos.x;
				addOrSub = 1;
				setBaseImage = 8;
				break;

			case 2: //上
				XorY = &nextMapPos.y;
				addOrSub = -1;
				setBaseImage = 9;
				break;

			case 3: //下
				XorY = &nextMapPos.y;
				addOrSub = 1;
				setBaseImage = 9;
				break;
			}

			*XorY += addOrSub;

			while (stage->mapData[nextMapPos.y][nextMapPos.x] == stage->Space ||
				   stage->mapData[nextMapPos.y][nextMapPos.x] == stage->Bomb)
			{
				if (stage->mapData[nextMapPos.y][nextMapPos.x] == -1) //普通の地面
				{
					stage->mapData[nextMapPos.y][nextMapPos.x] = stage->Explosion;
					auto newbomb = Bomb::Object::Create(true);
					newbomb->state = Death;
					newbomb->image.baseImageNum = setBaseImage;
					newbomb->pos = { float(nextMapPos.x * 32 + 16), float(nextMapPos.y * 32 + 16) };
					newbomb->bombMapPos = nextMapPos;

					--fireLength;
					*XorY += addOrSub;
					if (fireLength <= 0 || stage->mapData[nextMapPos.y][nextMapPos.x] == stage->Explosion)
					{
						newbomb->image.baseImageNum = 4 + i;
						break;
					}
				}
				else		//爆弾に接触
				{
					stage->mapData[nextMapPos.y][nextMapPos.x] = stage->CausingEx;
				}
			}
		}
	}

	//-------------------------------------------------------------------
	//爆発中の更新
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
	//爆弾の消滅
	void Object::DeleteBomb()
	{
		if (image.baseImageNum == 3)
		{
			if (auto player = ge->GetTask_One_GN<Player::Object>("本編", "プレイヤー"))
				--player->bombNum;
		}

		if (auto stage = ge->GetTask_One_GN<Stage::Object>("ステージ", "統括"))
			stage->mapData[bombMapPos.y][bombMapPos.x] = -1;

		Kill();
	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
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
	//リソースクラスの生成
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