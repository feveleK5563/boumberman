//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Stage.h"
#include  "Task_Block.h"
#include  "Task_Bomb.h"
#include  "Task_Player.h"

namespace  Stage
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		render2D_Priority[1] = 0.8f;
		StageSet();

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
	}

	//-------------------------------------------------------------------
	//ステージの設定
	void Object::StageSet()
	{
		//マップデータ読み込み(仮)
		int mpd[MapHeight][MapWidth] = {
			{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
			{ 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0 },
			{ 0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0 },
			{ 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0 },
			{ 0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0 },
			{ 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0 },
			{ 0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0, -1,  0 },
			{ 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0 },
			{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		};

		//上記のマップデータを元にブロックを作成
		for (int y = 0; y < MapHeight; ++y)
		{
			for (int x = 0; x < MapWidth; ++x)
			{
				mapData[y][x] = mpd[y][x];
				auto block = Block::Object::Create(true);
				block->pos = ML::Vec2(float(x * 32), float(y * 32));
				block->mapPos.x = x;
				block->mapPos.y = y;

				BreakBlockSet(x, y);

				block->blockNum = mapData[y][x];
				block->image.baseImageNum = block->blockNum;
			}
		}
	}

	//-------------------------------------------------------------------
	//破壊可能なブロックを配置する
	void Object::BreakBlockSet(const int x, const int y)
	{
		auto player = ge->GetTask_Group_GN<Player::Object>("本編", "プレイヤー");
		if (player == nullptr)
			return;

		//配置不可エリアを除外する
		if (mapData[y][x] != -1)
		{
			return;
		}
		for (auto it = player->begin(); it != player->end(); ++it)
		{
			if ((*it)->mapPos.x - 1 <= x && x <= (*it)->mapPos.x + 1 &&
				(*it)->mapPos.y - 1 <= y && y <= (*it)->mapPos.y + 1)
				return;
		}

		if (rand() % 3)
		{
			mapData[y][x] = BreakBlock;
		}
	}

	//-------------------------------------------------------------------
	//爆弾を設置
	bool Object::SetBomb(const ML::Vec2& plyPos)
	{
		POINT bmp = {
			int(plyPos.x / 32),
			int(plyPos.y / 32)
		};

		if (mapData[bmp.y][bmp.x] == -1)
		{
			mapData[bmp.y][bmp.x] = -2;
			auto bm = Bomb::Object::Create(true);
			bm->mapPos = bmp;
			bm->pos = { float(bmp.x * 32 + 16), float(bmp.y * 32 + 16) };

			return true;
		}
		else
		{
			return false;
		}
	}

	//-------------------------------------------------------------------
	//マップとの当たり判定
	bool Object::MapHitCheck(const ML::Box2D& hitBase)
	{
		RECT hb = { hitBase.x, hitBase.y,
					hitBase.x + hitBase.w, hitBase.y + hitBase.h };

		//矩形がマップ外に出ていたら丸め込みを行う
		RECT mb = { 0, 0, 32 * MapWidth, 32 * MapHeight };
		if (hb.left < mb.left) { hb.left = mb.left; }
		if (hb.top < mb.top) { hb.top = mb.top; }
		if (hb.right > mb.right) { hb.right = mb.right; }

		//ループ範囲調整
		int sx, sy, ex, ey;
		sx = hb.left / 32;
		sy = hb.top / 32;
		ex = (hb.right - 1) / 32;
		ey = (hb.bottom - 1) / 32;

		//範囲内の障害物を探す
		for (int y = sy; y <= ey; ++y)
		{
			for (int x = sx; x <= ex; ++x)
			{
				if (mapData[y][x] == NoBreakBlock ||
					mapData[y][x] == BreakBlock)
					return true;
			}
		}
		return false;
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