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
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imageName = "Bomb";
		DG::Image_Create(imageName, "./data/image/Bomb.png");
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
		render2D_Priority[1] = 0.5f;

		bombMapPos = { 0, 0 };
		state = State1;
		
		image.ImageCreate32x32(0, 0, 3, 1);
		image.ImageCreate32x32(0, 1, 7, 4);
		image.draw = { -16, -16, 32, 32 };

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
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		image.ImageRender(pos, res->imageName);
	}

	//-------------------------------------------------------------------
	//���������̍X�V
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

			//�U��
			if (auto stage = ge->GetTask_One_GN<Stage::Object>("�X�e�[�W", "����"))
			{
				if (stage->mapData[bombMapPos.y][bombMapPos.x] == stage->CausingEx)
					cntTime = 135;

			}

		}
	}

	//-------------------------------------------------------------------
	//�����̊J�n
	void Object::StartBomb()
	{
		image.baseImageNum = 3;
		state = Death;

		auto player = ge->GetTask_One_GN<Player::Object>("�{��", "�v���C���[");
		auto stage = ge->GetTask_One_GN<Stage::Object>("�X�e�[�W", "����");
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
			case 0:	//��
				XorY = &nextMapPos.x;
				addOrSub = -1;
				setBaseImage = 8;
				break;

			case 1: //�E
				XorY = &nextMapPos.x;
				addOrSub = 1;
				setBaseImage = 8;
				break;

			case 2: //��
				XorY = &nextMapPos.y;
				addOrSub = -1;
				setBaseImage = 9;
				break;

			case 3: //��
				XorY = &nextMapPos.y;
				addOrSub = 1;
				setBaseImage = 9;
				break;
			}

			*XorY += addOrSub;

			while (stage->mapData[nextMapPos.y][nextMapPos.x] == stage->Space ||
				   stage->mapData[nextMapPos.y][nextMapPos.x] == stage->Bomb)
			{
				if (stage->mapData[nextMapPos.y][nextMapPos.x] == -1) //���ʂ̒n��
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
				else		//���e�ɐڐG
				{
					stage->mapData[nextMapPos.y][nextMapPos.x] = stage->CausingEx;
				}
			}
		}
	}

	//-------------------------------------------------------------------
	//�������̍X�V
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
	//���e�̏���
	void Object::DeleteBomb()
	{
		if (image.baseImageNum == 3)
		{
			if (auto player = ge->GetTask_One_GN<Player::Object>("�{��", "�v���C���["))
				--player->bombNum;
		}

		if (auto stage = ge->GetTask_One_GN<Stage::Object>("�X�e�[�W", "����"))
			stage->mapData[bombMapPos.y][bombMapPos.x] = -1;

		Kill();
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