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
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imageName = "Player";
		DG::Image_Create(imageName, "./data/image/Player.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		DG::Image_Erase(imageName);
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
		
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		image.ImageErase();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
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
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (state != Non)
			image.ImageRender(pos, res->imageName);
	}

	//-------------------------------------------------------------------
	//�R���g���[���e�[�u��
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
	//�{�^�����͂ɉ����čs������
	void Object::MovePlayer()
	{
		auto in = DI::GPad_GetState("P" + to_string(playerNum));

		//����
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

		//���e�o��
		if (in.B1.down && bombNum < MaxBombNum)
		{
			if (auto stage = ge->GetTask_One_GN<Stage::Object>("�X�e�[�W", "����"))
			{
				if (stage->SetBomb(pos, playerNum))
					++bombNum;
			}
		}
	}

	//-------------------------------------------------------------------
	//���e�E�G�Ƃ̓����蔻��
	void Object::HitBombOREnemy()
	{
		auto stage = ge->GetTask_One_GN<Stage::Object>("�X�e�[�W", "����");
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
	//�ʏ펞�̃A�j���[�V����
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
	//���S���̃A�j���[�V����
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