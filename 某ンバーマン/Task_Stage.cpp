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
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		render2D_Priority[1] = 0.8f;
		StageSet();

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
	}

	//-------------------------------------------------------------------
	//�X�e�[�W�̐ݒ�
	void Object::StageSet()
	{
		//�}�b�v�f�[�^�ǂݍ���(��)
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

		//��L�̃}�b�v�f�[�^�����Ƀu���b�N���쐬
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
	//�j��\�ȃu���b�N��z�u����
	void Object::BreakBlockSet(const int x, const int y)
	{
		auto player = ge->GetTask_Group_GN<Player::Object>("�{��", "�v���C���[");
		if (player == nullptr)
			return;

		//�z�u�s�G���A�����O����
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
	//���e��ݒu
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
	//�}�b�v�Ƃ̓����蔻��
	bool Object::MapHitCheck(const ML::Box2D& hitBase)
	{
		RECT hb = { hitBase.x, hitBase.y,
					hitBase.x + hitBase.w, hitBase.y + hitBase.h };

		//��`���}�b�v�O�ɏo�Ă�����ۂߍ��݂��s��
		RECT mb = { 0, 0, 32 * MapWidth, 32 * MapHeight };
		if (hb.left < mb.left) { hb.left = mb.left; }
		if (hb.top < mb.top) { hb.top = mb.top; }
		if (hb.right > mb.right) { hb.right = mb.right; }

		//���[�v�͈͒���
		int sx, sy, ex, ey;
		sx = hb.left / 32;
		sy = hb.top / 32;
		ex = (hb.right - 1) / 32;
		ey = (hb.bottom - 1) / 32;

		//�͈͓��̏�Q����T��
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

	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
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
	//���\�[�X�N���X�̐���
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