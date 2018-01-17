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
		image.ImageCreate32x32(0, 4, 3, 3);
		image.ImageCreate32x32(0, 7, 6, 1);
		image.drawPos.x = 16;
		image.drawPos.y = 24;

		state = State1;
		hitBase = { -10, -11, 20, 17 };
		image.baseImageNum = 6;
		angleLRUD = Down;
		
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
		image.defImageNum = (playerNum - 1) * 15;
		switch (state)
		{
		case BChara::State1:
			MovePlayer();
			CheckHitMove();
			NomalAnimation();
			HitBombOREnemy();
			break;

		case BChara::Death:
			DeathAnimation();
			break;

		default:
			return;
		}

		mapPos.x = int(pos.x / 32);
		mapPos.y = int(pos.y / 32);
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (state != Non)
			image.ImageRender(pos, res->imageName);
	}

	//-------------------------------------------------------------------
	//コントロールテーブル
	int Object::InputBTable(DI::VGamePad in)
	{
		int table[3][3] = {
			{ 2,  2,  2},
			{ 0, -1,  1},
			{ 3,  3,  3}
		};
		int tableX = 1, tableY = 1;

		if (in.LStick.L.on) { --tableX; }
		if (in.LStick.R.on) { ++tableX; }
		if (in.LStick.U.on) { --tableY; }
		if (in.LStick.D.on) { ++tableY; }

		return table[tableY][tableX];
	}

	//-------------------------------------------------------------------
	//ボタン入力に応じて行う処理
	void Object::MovePlayer()
	{
		auto in = DI::GPad_GetState("P" + to_string(playerNum));

		//動作
		speed = { 0.f, 0.f };
		switch (InputBTable(in))
		{
		case 0:	//L
			speed.x = -1.5f;
			angleLRUD = Left;
			break;

		case 1: //R
			speed.x = 1.5f;
			angleLRUD = Right;
			break;

		case 2: //U
			speed.y = -1.5f;
			angleLRUD = Up;
			break;

		case 3: //D
			speed.y = 1.5f;
			angleLRUD = Down;
			break;
		}

		//爆弾出現
		if (in.B1.down && bombNum < MaxBombNum)
		{
			if (auto stage = ge->GetTask_One_GN<Stage::Object>("ステージ", "統括"))
			{
				if (stage->SetBomb(pos, playerNum))
					++bombNum;
			}
		}
	}

	//-------------------------------------------------------------------
	//爆弾・敵との当たり判定
	void Object::HitBombOREnemy()
	{
		auto stage = ge->GetTask_One_GN<Stage::Object>("ステージ", "統括");
		if (stage == nullptr)
			return;

		if (stage->mapData[mapPos.y][mapPos.x] == stage->Explosion)
		{
			state = Death;
			image.baseImageNum = 9;
			image.animCnt = 0;
			image.animTurn = false;
			cntTime = 0;
		}
	}

	//-------------------------------------------------------------------
	//通常時のアニメーション
	void Object::NomalAnimation()
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

	//-------------------------------------------------------------------
	//死亡時のアニメーション
	void Object::DeathAnimation()
	{
		if (cntTime > 60)
		{
			image.animCnt = float((cntTime - 55) / 5 % 6);

			if (!image.animCnt)
				state = Non;
		}
		++cntTime;
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