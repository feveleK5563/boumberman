//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Stage.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Player";
		DG::Image_Create(imageName, "./data/image/Player.png");
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
		render2D_Priority[1] = 0.3f;

		MaxBombNum = 10;
		bombNum = 0;
		bombPower = 2;
		setSpeed = 1.5f;

		image.ImageCreate32x32(0, 0, 3, 3);
		image.ImageCreate32x32(0, 3, 6, 1);
		image.draw = { -16, -26, 32, 32 };

		state = State1;
		pos = { 48.f, 48.f };
		hitBase = { -10, -11, 20, 17 };
		image.baseImageNum = 6;
		angleLRUD = Right;
		
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
		case BChara::State1:
			MovePlayer();
			CheckHitMove();
			Animation();
			break;

		case BChara::Death:
			break;

		default:
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
	//ボタン入力に応じて行う処理
	void Object::MovePlayer()
	{
		auto in = DI::GPad_GetState("P1");

		//動作
		if (in.LStick.L.down) { speed.x = -setSpeed; speed.y = 0; }
		if (in.LStick.R.down) { speed.x =  setSpeed; speed.y = 0; }
		if (in.LStick.L.up || in.LStick.R.up)
		{
			speed.x = 0;
			if (in.LStick.U.on) { speed.y = -setSpeed; }
			if (in.LStick.D.on) { speed.y =  setSpeed; }
		}

		if (in.LStick.U.down) { speed.y = -setSpeed; speed.x = 0; }
		if (in.LStick.D.down) { speed.y =  setSpeed; speed.x = 0; }
		if (in.LStick.U.up || in.LStick.D.up)
		{
			speed.y = 0;
			if (in.LStick.L.on) { speed.x = -setSpeed; }
			if (in.LStick.R.on) { speed.x =  setSpeed; }
		}

		if (speed.x < 0) { angleLRUD = Left; }
		if (speed.x > 0) { angleLRUD = Right; }
		if (speed.y < 0) { angleLRUD = Up; }
		if (speed.y > 0) { angleLRUD = Down; }


		//爆弾出現
		if (in.B1.down && bombNum < MaxBombNum)
		{
			if (auto stage = ge->GetTask_One_GN<Stage::Object>("ステージ", "統括"))
			{
				if (stage->SetBomb(pos))
					++bombNum;
			}
		}
	}

	//-------------------------------------------------------------------
	//アニメーション
	void Object::Animation()
	{
		switch (angleLRUD)
		{
		case Left:
			image.baseImageNum = 3;
			image.animTurn = true;
			break;

		case Right:
			image.baseImageNum = 3;
			image.animTurn = false;
			break;

		case Up:
			image.baseImageNum = 0;
			image.animTurn = false;
			break;

		case Down:
			image.baseImageNum = 6;
			image.animTurn = false;
			break;
		}

		if (fabsf(speed.x) > 0.f ||
			fabsf(speed.y) > 0.f)
		{
			++cntTime;
			image.animCnt = float(AnimTable[(cntTime / 5) % 4]);
		}
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