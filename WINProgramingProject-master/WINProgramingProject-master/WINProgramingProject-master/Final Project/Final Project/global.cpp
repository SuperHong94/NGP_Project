#include "global.h"

int sj_Timer = 0;
RECT Player_1;
RECT Player_2;
RECT WindowSize;
RECT Energybar;
RECT Energybar2;
HBITMAP Laser_Boom;
HBITMAP Circle_Boom;
HBITMAP Teleport;
HBITMAP Teleport2;
HBITMAP PLAYER_1;
HBITMAP PLAYER_2;

int PLAYER1_HIT;
int PLAYER2_HIT;


bool Tp_2;
bool Tp;
RECT tmp2;
RECT tmp;
PLAYERDIRECTION PLAYER1DR;
PLAYERDIRECTION PLAYER2DR;
bool POWEROVERWHELMING;
int score;

bool g_isMulti = false;

bool Crush(RECT* player, int LX, int LY, int RX, int RY) //충돌!!LY는 LeftY의 준말 plyaer하고 폭탄의 범위랑 충돌처리할꺼임  //충돌하면 true리턴
{
	int playerWidth = (player->bottom - player->top) / 2;  //높이구해서 2로 나눔 player중점좌표구할려고하는거임
	int playerX = player->left + playerWidth; //player의 중점X좌표
	int playerY = player->top + playerWidth;      //plyaer의 중점 Y좌표
	int boomLX = LX - playerWidth;
	int boomLY = LY - playerWidth;
	int boomRX = playerWidth + RX;
	int boomRY = RY + playerWidth;
	if ((playerX >= boomLX && playerX <= boomRX) && (playerY >= boomLY && playerY <= boomRY))
		return true;
	else
		return false;

}

bool IsPointInCircle(int CenterX, int CenterY, int Radius, int X, int Y)
{
	float deltaX = (float)CenterX - (float)X;
	float deltaY = (float)CenterY - (float)Y;
	float length = sqrt(deltaX*deltaX + deltaY * deltaY);

	if (length > Radius)
		return FALSE;
	return TRUE;
}

bool CircleCrush(RECT* player, int LX, int LY, int RX, int RY)
{
	int CenterX = (LX + RX) / 2;
	int CenterY = (LY + RY) / 2;
	int radius = CenterX - LX;


	if ((player->left <= CenterX && CenterX <= player->right) || (
		player->top <= CenterY && CenterY <= player->bottom))
	{
		RECT tmpRect{
		   player->left - radius,
		   player->top - radius,
		   player->right + radius,
		   player->bottom + radius
		};
		if ((tmpRect.left < CenterX&&CenterX < tmpRect.right) &&
			(tmpRect.top < CenterY&&CenterY < tmpRect.bottom))
			return TRUE;
	}
	else
	{
		if (IsPointInCircle(CenterX, CenterY, radius, player->left, player->top))
			return TRUE;

		if (IsPointInCircle(CenterX, CenterY, radius, player->left, player->bottom))
			return TRUE;

		if (IsPointInCircle(CenterX, CenterY, radius, player->right, player->top))
			return TRUE;

		if (IsPointInCircle(CenterX, CenterY, radius, player->right, player->top))
			return TRUE;
	}
	return FALSE;
}

bool OutOfRange(Boom* boom)
{
	if (boom == NULL)
		return FALSE;

	if (boom->leftTop.x < WindowSize.right && boom->rightBottom.x > WindowSize.left &&
		boom->leftTop.y < WindowSize.bottom && boom->rightBottom.y > WindowSize.top)
		return FALSE;

	else
		return TRUE;
}
void InitBoom(Boom& boom, int lX, int lY, int rX, int rY)
{
	boom.leftTop.x = lX;
	boom.leftTop.y = lY;
	boom.rightBottom.x = rX;
	boom.rightBottom.y = rY;
}

void addBoom(Boom* head, EShape tmp_boomShape, int tmpLeftTop_x, int tmpLeftTop_y, int tmpRightBottom_x, int tmpRightBottom_y)
{
	Boom* p = new Boom;
	p->nextBoom = head->nextBoom;
	p->boomShape = tmp_boomShape;
	p->leftTop.x = tmpLeftTop_x;
	p->leftTop.y = tmpLeftTop_y;
	p->rightBottom.x = tmpRightBottom_x;
	p->rightBottom.y = tmpRightBottom_y;
	p->boomAnimaition = 0;
	head->nextBoom = p;
}

void SunBoom_SJ(HDC hDC, Boom* head, int x, int y)
{
	addBoom(head, Bullet_Down, x - 10, y - 10, x + 10, y + 10);
	addBoom(head, Bullet_Right, x - 10, y - 10, x + 10, y + 10);
	addBoom(head, Bullet_Left, x - 10, y - 10, x + 10, y + 10);
	addBoom(head, Bullet_Up, x - 10, y - 10, x + 10, y + 10);
	addBoom(head, Bullet_UpRight, x - 10, y - 10, x + 10, y + 10);
	addBoom(head, Bullet_UpLeft, x - 10, y - 10, x + 10, y + 10);
	addBoom(head, Bullet_DownLeft, x - 10, y - 10, x + 10, y + 10);
	addBoom(head, Bullet_DownRight, x - 10, y - 10, x + 10, y + 10);
}

void Doughnut(HDC hDC, Boom* head, int x, int y, int width)
{

	addBoom(head, MyDoughnut1, x, y, x + width, y + 20);
	addBoom(head, MyDoughnut2, x, y, x + 20, y + width);
	addBoom(head, MyDoughnut3, x, y + width - 20, x + width, y + width);
	addBoom(head, MyDoughnut4, x + width - 20, y, x + width, y + width);
}

void NormalBullet(HDC hDC, Boom* boom)
{
	if (boom == NULL)
		return;
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;
	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	oldPen = (HPEN)SelectObject(hDC, hPen);
	switch (boom->boomShape) {
		//case Boom_Sun:
		//	Ellipse(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x, boom->rightBottom.y);
		//	break;
	case MyDoughnut1:
	case MyDoughnut2:
	case MyDoughnut3:
	case MyDoughnut4:
		Rectangle(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x, boom->rightBottom.y);

		break;
	case Bullet_Up:
		Ellipse(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x, boom->rightBottom.y);
		break;
	case Bullet_Down:
	case Bullet_Right:
	case Bullet_Left:
	case Bullet_UpRight:
	case Bullet_UpLeft:
	case Bullet_DownRight:
	case Bullet_DownLeft:
	case Boom_Meet:
		Ellipse(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x, boom->rightBottom.y);

		break;
	}
	(HPEN)SelectObject(hDC, oldPen);
	DeleteObject(hPen);
	(HBRUSH)SelectObject(hDC, oldBrush);
	DeleteObject(hBrush);
}

void deleteBoom(Boom* target)
{
	Boom* Delete = target->nextBoom;

	target->nextBoom = Delete->nextBoom;

	free(Delete);
}

void setBoomPosition(Boom* head)
{
	Boom* p;

	for (p = head; p->nextBoom != NULL; p = p->nextBoom)
	{
		p->nextBoom->setPosition();
	}
}

void setAnimation(Boom* head)
{
	Boom* p;

	for (p = head; p->nextBoom != NULL; p = p->nextBoom)
	{
		if (p->nextBoom->boomAnimaition != -1)
			p->nextBoom->boomAnimaition++;
	}
}

void Boom::setPosition()
{
	switch (boomShape)
	{
	case Boom_Sun:

		break;
	case MyDoughnut1:
		leftTop.x--;
		leftTop.y--;
		rightBottom.x++;
		rightBottom.y--;
		break;
	case MyDoughnut2:
		leftTop.x--;
		leftTop.y--;
		rightBottom.x--;
		rightBottom.y++;
		break;
	case MyDoughnut3:
		leftTop.x--;
		leftTop.y++;
		rightBottom.x++;
		rightBottom.y++;
		break;
	case MyDoughnut4:
		leftTop.x++;
		leftTop.y--;
		rightBottom.x++;
		rightBottom.y++;
		break;

	case MyRectangle:

		break;
	case MyTriangle:

		break;
	case Boom_Circle:
		if (boomAnimaition == 100)
		{
			leftTop.x -= 25;
			leftTop.y -= 25;
			rightBottom.x += 25;
			rightBottom.y += 25;
		}

		else if (boomAnimaition > 100)
		{
			leftTop.x += 10;
			leftTop.y += 10;
			rightBottom.x -= 10;
			rightBottom.y -= 10;
			if (leftTop.x >= rightBottom.x)
				boomAnimaition = -1;
		}
		break;
	case Boom_Rectangle:
		if (boomAnimaition == 100)
		{
			leftTop.x -= 25;
			leftTop.y -= 25;
			rightBottom.x += 25;
			rightBottom.y += 25;
		}

		else if (boomAnimaition > 100)
		{
			leftTop.x += 10;
			leftTop.y += 10;
			rightBottom.x -= 10;
			rightBottom.y -= 10;
			if (leftTop.x >= rightBottom.x)
				boomAnimaition = -1;
		}
		break;
	case Boom_Laser:
		if (boomAnimaition == 100)
		{
			leftTop.y -= 25;
			rightBottom.y += 25;
		}
		else if (boomAnimaition > 100)
		{
			leftTop.y += 2;
			rightBottom.y -= 2;
			if (leftTop.y >= rightBottom.y)
				boomAnimaition = -1;
		}
		break;
	case Boom_LeftLaser:
		if (boomAnimaition == 100)
		{
			leftTop.x = 1100;
		}
		else if (boomAnimaition > 100 && boomAnimaition < 110)
		{
			leftTop.x -= 2;
		}
		else if (boomAnimaition == 110)
		{
			leftTop.x = -100;
		}
		else if (boomAnimaition > 110)
		{
			rightBottom.x -= 100;
			if (rightBottom.x <= -100)
				boomAnimaition = -1;
		}
		break;
	case Boom_RightLaser:
		if (boomAnimaition == 100)
		{
			rightBottom.x = 0;
		}
		else if (boomAnimaition > 100 && boomAnimaition < 110)
		{
			rightBottom.x += 2;
		}
		else if (boomAnimaition == 110)
		{
			rightBottom.x = 1300;
		}
		else if (boomAnimaition > 110)
		{
			leftTop.x += 100;
			if (leftTop.x >= 1300)
				boomAnimaition = -1;
		}
		break;
	case Boom_DownLaser:
		if (boomAnimaition == 100)
		{
			rightBottom.y = 0;
		}
		else if (boomAnimaition > 100 && boomAnimaition < 110)
		{
			rightBottom.y += 2;
		}
		else if (boomAnimaition == 110)
		{
			rightBottom.y = 900;
		}
		else if (boomAnimaition > 110)
		{
			leftTop.y += 100;
			if (leftTop.y >= 900)
				boomAnimaition = -1;
		}
		break;
	case Boom_UpLaser:
		if (boomAnimaition == 100)
		{
			leftTop.y = 900;
		}
		else if (boomAnimaition > 100 && boomAnimaition < 110)
		{
			leftTop.y -= 2;
		}
		else if (boomAnimaition == 110)
		{
			leftTop.y = -100;
		}
		else if (boomAnimaition > 110)
		{
			rightBottom.y -= 100;
			if (rightBottom.y <= -100)
				boomAnimaition = -1;
		}
		break;
	case Bullet_Up:
		leftTop.y -= 3;
		rightBottom.y -= 3;
		break;
	case Bullet_Down:
		leftTop.y += 3;
		rightBottom.y += 3;
		break;
	case Bullet_Right:
		leftTop.x += 3;
		rightBottom.x += 3;
		break;
	case Bullet_Left:
		leftTop.x -= 3;
		rightBottom.x -= 3;
		break;
	case Bullet_UpRight:
		leftTop.y -= 3;
		leftTop.x += 3;
		rightBottom.x += 3;
		rightBottom.y -= 3;
		break;
	case Bullet_UpLeft:
		leftTop.y -= 3;
		rightBottom.y -= 3;
		leftTop.x -= 3;
		rightBottom.x -= 3;
		break;
	case Bullet_DownRight:
		leftTop.y += 3;
		rightBottom.y += 3;
		leftTop.x += 3;
		rightBottom.x += 3;
		break;
	case Bullet_DownLeft:
		leftTop.y += 3;
		rightBottom.y += 3;
		leftTop.x -= 3;
		rightBottom.x -= 3;
		break;
	case Boom_Laser2:
		if (boomAnimaition == 100)
		{
			leftTop.x -= 25;
			rightBottom.x += 25;
		}
		else if (boomAnimaition > 100)
		{
			leftTop.x += 2;
			rightBottom.x -= 2;
			if (leftTop.x >= rightBottom.x)
				boomAnimaition = -1;
		}
		break;
	default:

		break;
	}
}


void CircleBoom(HDC hDC, HINSTANCE g_hInst, Boom* boom)
{
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;

	HDC memDC;

	if (boom->boomAnimaition < 100)
	{
		memDC = CreateCompatibleDC(hDC);
		(HBITMAP)SelectObject(memDC, Circle_Boom);
		TransparentBlt(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x - boom->leftTop.x, boom->rightBottom.y - boom->leftTop.y, memDC, 0, 0, 1024, 1024, RGB(0, 0, 0));
		DeleteDC(memDC);
	}

	else
	{
		switch (boom->boomAnimaition % 2)
		{
		case 0:
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			Ellipse(hDC, boom->leftTop.x, boom->rightBottom.y, boom->rightBottom.x, boom->leftTop.y);
			SelectObject(hDC, oldBrush);
			DeleteObject(hBrush);
			break;
		case 1:
			hBrush = CreateSolidBrush(RGB(255, 255, 255));
			oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			Ellipse(hDC, boom->leftTop.x, boom->rightBottom.y, boom->rightBottom.x, boom->leftTop.y);
			SelectObject(hDC, oldBrush);
			DeleteObject(hBrush);
			break;
		}
	}
}

void LaserBoom(HDC hDC, HINSTANCE g_hInst, Boom* boom)
{
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;

	HDC memDC;


	if (boom->boomAnimaition < 100)
	{
		memDC = CreateCompatibleDC(hDC);
		(HBITMAP)SelectObject(memDC, Laser_Boom);
		TransparentBlt(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x - boom->leftTop.x, boom->rightBottom.y - boom->leftTop.y, memDC, 0, 0, 1024, 1024, RGB(0, 0, 0));
		DeleteDC(memDC);
	}
	else
	{
		switch (boom->boomAnimaition % 2)
		{
		case 0:
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			Rectangle(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x, boom->rightBottom.y);
			SelectObject(hDC, oldBrush);
			DeleteObject(hBrush);
			break;
		case 1:
			hBrush = CreateSolidBrush(RGB(255, 255, 255));
			oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			Rectangle(hDC, boom->leftTop.x, boom->leftTop.y, boom->rightBottom.x, boom->rightBottom.y);
			SelectObject(hDC, oldBrush);
			DeleteObject(hBrush);
			break;
		}
	}
}

void printBoomAnimation(HDC hDC, HINSTANCE g_hInst, Boom* head)
{
	Boom* p;

	for (p = head; p->nextBoom != NULL; p = p->nextBoom)
	{
		switch (p->nextBoom->boomShape)
		{
		case 0:
			break;
		case MyDoughnut1:
		case MyDoughnut2:
		case MyDoughnut3:
		case MyDoughnut4:
			NormalBullet(hDC, p->nextBoom);
			break;
		case Boom_Circle:
			CircleBoom(hDC, g_hInst, p->nextBoom);
			break;
		case Bullet_Up:
		case Bullet_Down:
		case Bullet_Right:
		case Bullet_Left:
		case Bullet_UpRight:
		case Bullet_DownRight:
		case Bullet_DownLeft:
		case Bullet_UpLeft:
		case Boom_Meet:
			NormalBullet(hDC, p->nextBoom);
			break;
		case Boom_Laser:
		case Boom_Laser2:
		case Boom_LeftLaser:
		case Boom_RightLaser:
		case Boom_UpLaser:
		case Boom_DownLaser:
		case Boom_Rectangle:
			LaserBoom(hDC, g_hInst, p->nextBoom);
			break;
		default:
			break;
		}
	}
}

void CheckBullet(Boom* head)
{
	Boom* p;
	for (p = head; p->nextBoom != NULL; p = p->nextBoom)
	{

		while (OutOfRange(p->nextBoom))
		{
			deleteBoom(p);
			if (p->nextBoom == NULL)
				break;
		}
		if (p->nextBoom == NULL)
			break;
	}
}

void CheckBoom(Boom* head)
{
	Boom* p;

	for (p = head; p->nextBoom != NULL; p = p->nextBoom)
	{

		if (p->nextBoom->boomShape >= 1 && p->nextBoom->boomShape <= 4)
		{
			while (OutOfRange(p->nextBoom))
			{
				deleteBoom(p);
				if (p->nextBoom == NULL)
					break;
			}
		}
		else
		{
			while (p->nextBoom->boomAnimaition == -1)
			{
				deleteBoom(p);
				if (p->nextBoom == NULL)
					break;
			}
		}


		if (p->nextBoom == NULL)
			break;
	}
}

void Animation(HDC hDC, HINSTANCE g_hInst, Boom* head, Boom* bullet_head)
{
	HDC memDC;
	HBRUSH hBrush, oldBrush;

	printBoomAnimation(hDC, g_hInst, head);
	printBoomAnimation(hDC, g_hInst, bullet_head);

	memDC = CreateCompatibleDC(hDC);
	if (Tp)
	{
		(HBITMAP)SelectObject(memDC, Teleport);
		TransparentBlt(hDC, tmp.left, tmp.top, tmp.right - tmp.left, tmp.bottom - tmp.top, memDC, 0, 0, 50, 50, RGB(0, 0, 0));
	}

	if (Tp_2)
	{
		(HBITMAP)SelectObject(memDC, Teleport2);
		TransparentBlt(hDC, tmp2.left, tmp2.top, tmp2.right - tmp2.left, tmp2.bottom - tmp2.top, memDC, 0, 0, 50, 50, RGB(0, 0, 0));
	}

	if (PLAYER1_HIT > 0)
	{
		switch (PLAYER1_HIT%2)
		{
		case 0:
			hBrush = CreateSolidBrush(RGB(126, 66, 31));
			oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
			Rectangle(hDC, Player_1.left, Player_1.top, Player_1.right, Player_1.bottom);
			SelectObject(hDC, oldBrush);
			DeleteObject(hBrush);
			break;
		case 1:
			(HBITMAP)SelectObject(memDC, PLAYER_1);
			StretchBlt(hDC, Player_1.left, Player_1.top, Player_1.right - Player_1.left, Player_1.bottom - Player_1.top, memDC, 0, 0, 50, 50, SRCCOPY);
			break;
		default:
			break;
		}
		--PLAYER1_HIT;
	}

	else
	{
		(HBITMAP)SelectObject(memDC, PLAYER_1);
		StretchBlt(hDC, Player_1.left, Player_1.top, Player_1.right - Player_1.left, Player_1.bottom - Player_1.top, memDC, 0, 0, 50, 50, SRCCOPY);
	}

	if (g_isMulti) {
		if (PLAYER2_HIT > 0)
		{
			switch (PLAYER2_HIT % 2)
			{
			case 0:
				hBrush = CreateSolidBrush(RGB(255, 0, 204));
				oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
				Rectangle(hDC, Player_2.left, Player_2.top, Player_2.right, Player_2.bottom);
				SelectObject(hDC, oldBrush);
				DeleteObject(hBrush);
				break;
			case 1:
				(HBITMAP)SelectObject(memDC, PLAYER_2);
				StretchBlt(hDC, Player_2.left, Player_2.top, Player_2.right - Player_2.left, Player_2.bottom - Player_2.top, memDC, 0, 0, 50, 50, SRCCOPY);
				break;
			default:
				break;
			}
			--PLAYER2_HIT;
		}
		else
		{
			(HBITMAP)SelectObject(memDC, PLAYER_2);
			StretchBlt(hDC, Player_2.left, Player_2.top, Player_2.right - Player_2.left, Player_2.bottom - Player_2.top, memDC, 0, 0, 50, 50, SRCCOPY);
		}
	}
	DeleteDC(memDC);
}

void DrawEnergybar(HDC hDC, HINSTANCE hInst, EROUND& eRound)
{
	HDC mDC;
	BITMAP bit;
	HBITMAP hBit;
	HBRUSH hBrush, hBrush1;
	//BLENDFUNCTION m_BlendFuntion; //불투명;
	//m_BlendFuntion.BlendOp = AC_SRC_OVER;  //블렌딩 옵션_ACSRCC_OVER는 혼합옵션
	//m_BlendFuntion.AlphaFormat = 0;
	//m_BlendFuntion.AlphaFormat = 0;
	//m_BlendFuntion.SourceConstantAlpha = 150; //0은 완전 투명  255는 완전 불투명)
	hBrush = CreateSolidBrush(RGB(100, 100, 100));
	hBrush1 = CreateSolidBrush(RGB(0, 0, 100));
	(HBRUSH)SelectObject(hDC, hBrush);
	Rectangle(hDC, WindowSize.left, WindowSize.bottom - 20, WindowSize.right, WindowSize.bottom);
	(HBRUSH)SelectObject(hDC, hBrush1);
	Rectangle(hDC, Energybar.left, Energybar.bottom - 20, Energybar.right, Energybar.bottom);

	DeleteObject(hBrush);
	DeleteObject(hBrush1);

}

void DrawEnergybar2(HDC hDC, HINSTANCE hInst, EROUND& eRound)
{
	HDC mDC;
	BITMAP bit;
	HBITMAP hBit;
	HBRUSH hBrush, hBrush1;
	//BLENDFUNCTION m_BlendFuntion; //불투명;
	//m_BlendFuntion.BlendOp = AC_SRC_OVER;  //블렌딩 옵션_ACSRCC_OVER는 혼합옵션
	//m_BlendFuntion.AlphaFormat = 0;
	//m_BlendFuntion.AlphaFormat = 0;
	//m_BlendFuntion.SourceConstantAlpha = 150; //0은 완전 투명  255는 완전 불투명)
	hBrush = CreateSolidBrush(RGB(100, 100, 100));
	hBrush1 = CreateSolidBrush(RGB(0, 0, 100));
	(HBRUSH)SelectObject(hDC, hBrush);
	Rectangle(hDC, WindowSize.left, WindowSize.bottom - 40, WindowSize.right, WindowSize.bottom-20);
	(HBRUSH)SelectObject(hDC, hBrush1);
	Rectangle(hDC, Energybar2.left, Energybar2.bottom - 40, Energybar2.right, Energybar2.bottom-20);

	DeleteObject(hBrush);
	DeleteObject(hBrush1);

}

void CheckBulletCrush(Boom* head)
{
	Boom* p;

	for (p = head; p->nextBoom != NULL; p = p->nextBoom)
	{
		if (PLAYER1_HIT == 0 && Crush(&Player_1, p->nextBoom->leftTop.x, p->nextBoom->leftTop.y, p->nextBoom->rightBottom.x, p->nextBoom->rightBottom.y))
		{
			Energybar.right -= 200;
			PLAYER1_HIT = 50;
			effPlaySound(Damage);
		}
		//if (PLAYER2_HIT == 0 && Crush(&Player_2, p->nextBoom->leftTop.x, p->nextBoom->leftTop.y, p->nextBoom->rightBottom.x, p->nextBoom->rightBottom.y))
		//{
		//	Energybar2.right -= 200;
		//	PLAYER2_HIT = 50;
		//	effPlaySound(Damage);
		//}
	}
}

void CheckBoomCrush(Boom* head)
{
	Boom* p;

	for (p = head; p->nextBoom != NULL; p = p->nextBoom)
	{
		if ((PLAYER1_HIT == 0) && (p->nextBoom->boomAnimaition >= 100) && Crush(&Player_1, p->nextBoom->leftTop.x, p->nextBoom->leftTop.y, p->nextBoom->rightBottom.x, p->nextBoom->rightBottom.y))
		{
			Energybar.right -= 200;
			PLAYER1_HIT = 50;
			effPlaySound(Damage);
		}
		//if ((PLAYER2_HIT == 0) && (p->nextBoom->boomAnimaition >= 100) && Crush(&Player_2, p->nextBoom->leftTop.x, p->nextBoom->leftTop.y, p->nextBoom->rightBottom.x, p->nextBoom->rightBottom.y))
		//{
		//	Energybar2.right -= 200;
		//	PLAYER2_HIT = 50;
		//	effPlaySound(Damage);
		//}
	}
}

int DrawMenu(HDC hDC, EROUND& eRound, HINSTANCE hInst)
{
	char buffer[1000]; //초세는 타이머
	HFONT hFont;
	HBITMAP hBit;
	BITMAP bit;
	HDC mDC;
	switch (eRound)
	{
	case MAIN:
		mDC = CreateCompatibleDC(hDC);
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MAIN));
		GetObject(hBit, sizeof(bit), &bit);
		(HBITMAP)SelectObject(mDC, hBit);
		TransparentBlt(hDC, 0, 0, WindowSize.right, WindowSize.bottom, mDC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);
		DeleteDC(mDC);
		return 0;
	case HELP:
		mDC = CreateCompatibleDC(hDC);
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HELP));
		GetObject(hBit, sizeof(bit), &bit);
		(HBITMAP)SelectObject(mDC, hBit);
		TransparentBlt(hDC, 0, 0, WindowSize.right, WindowSize.bottom, mDC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);
		DeleteDC(mDC);
		return 0;
	case SelectPlay:
		mDC = CreateCompatibleDC(hDC);
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SELECTPLAY));
		GetObject(hBit, sizeof(bit), &bit);
		(HBITMAP)SelectObject(mDC, hBit);
		TransparentBlt(hDC, 0, 0, WindowSize.right, WindowSize.bottom, mDC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);
		DeleteDC(mDC);
		return 0;
	case Select:
		mDC = CreateCompatibleDC(hDC);
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SELECTSTAGE));
		GetObject(hBit, sizeof(bit), &bit);
		(HBITMAP)SelectObject(mDC, hBit);
		TransparentBlt(hDC, 0, 0, WindowSize.right, WindowSize.bottom, mDC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);
		DeleteDC(mDC);
		return 0;
	case YouDie:
		mDC = CreateCompatibleDC(hDC);
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_YOUDIE));
		GetObject(hBit, sizeof(BITMAP), &bit);
		(HBITMAP)SelectObject(mDC, hBit);
		TransparentBlt(hDC, 0, 0, WindowSize.right, WindowSize.bottom, mDC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);  //글씨만 출력
		DeleteDC(mDC);
		return 0;
	case YouWin:
		mDC = CreateCompatibleDC(hDC);
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_YOUWIN));
		GetObject(hBit, sizeof(BITMAP), &bit);
		(HBITMAP)SelectObject(mDC, hBit);
		TransparentBlt(hDC, 0, 0, WindowSize.right, WindowSize.bottom, mDC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);  //글씨만 출력

		hFont = CreateFont(100, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, L"궁서체");
		(HFONT)SelectObject(hDC, hFont);
		SetTextColor(hDC, RGB(255, 255, 255));
		SetBkColor(hDC, RGB(0, 0, 0));
		sprintf(buffer, "점수: %d", score);
		TextOutA(hDC, 600, 0, buffer, strlen(buffer));
		DeleteObject(hFont);
		DeleteDC(mDC);
		return 0;
	case Round1:

		return 1;
	case Round2:
		return 2;
	default:
		break;
	}
}


int DrawTelCnt(HDC hDC, EROUND& eRound, HINSTANCE hInst)
{
	char buffer[1000]; //초세는 타이머
	HFONT hFont;
	HBITMAP hBit;
	BITMAP bit;

	switch (eRound)
	{
	case Round1:
		hFont = CreateFont(50, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, L"궁서체");
		(HFONT)SelectObject(hDC, hFont);
		SetTextColor(hDC, RGB(255, 255, 255));
		SetBkColor(hDC, RGB(0, 0, 0));
		sprintf(buffer, "teleport: %d   dash: %d", p2TcpData.telCnt, p2TcpData.DashCnt);
		TextOutA(hDC, 600, 0, buffer, strlen(buffer));
		DeleteObject(hFont);


		return 0;
	case Round2:
		hFont = CreateFont(50, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, L"궁서체");
		(HFONT)SelectObject(hDC, hFont);
		SetTextColor(hDC, RGB(255, 255, 255));
		SetBkColor(hDC, RGB(0, 0, 0));
		sprintf(buffer, "teleport: %d   dash: %d", p2TcpData.telCnt, p2TcpData.DashCnt);
		TextOutA(hDC, 600, 0, buffer, strlen(buffer));
		DeleteObject(hFont);

		return 0;
	default:
		break;
	}
}

int ClickRange(LPARAM lParam, EROUND& eRound)
{
	int state = 0;
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	switch (eRound) {
	case MAIN:
		if ((x >= 190 && y >= 250) && (x <= 1000 && y <= 370))
		{
			eRound = SelectPlay;
			p1TcpData.sceneState = SelectPlay;
		}
		if ((x >= 190 && y >= 400) && (x <= 1000 && y <= 525))
		{
			eRound = HELP;
			p1TcpData.sceneState = HELP;
		}
		if ((x >= 190 && y >= 555) && (x <= 1000 && y <= 680))
			PostQuitMessage(0);
		break;
	case SelectPlay:
		if ((x >= 80 && y >= 100) && (x <= 1110 && y <= 260)) {
			g_isMulti = false;
			state = 0;
			eRound = Select;
			//p1TcpData.sceneState = Select;
			//p1TcpData.sceneState = Select;
		}
		if ((x >= 190 && y >= 300) && (x <= 1110 && y <= 460)) {
			g_isMulti = true;
			state = 1;
			eRound = Select;
			//p1TcpData.sceneState = Select;
		}
		if ((x >= 190 && y >= 500) && (x <= 1110 && y <= 660))
		{
			eRound = MAIN;
			p1TcpData.sceneState = MAIN;
		}
		break;
	case Select:
		//if (p1TcpData.sceneState == Round1 && p2TcpData.sceneState == Round1)
		//	eRound = Round1;

		//else if (p1TcpData.sceneState == Round2 && p2TcpData.sceneState == Round2)
		//	eRound = Round2;

		if ((x >= 80 && y >= 100) && (x <= 1110 && y <= 260)) {
			if (!g_isMulti)
				eRound = Round1;
			else
				p1TcpData.sceneState = Round1;
		}
		if ((x >= 190 && y >= 300) && (x <= 1110 && y <= 460)) {
			if (!g_isMulti)
				eRound = Round2;
			else
				p1TcpData.sceneState = Round2;
		}
		if ((x >= 190 && y >= 500) && (x <= 1110 && y <= 660))
		{
			eRound = SelectPlay;
			p1TcpData.sceneState = SelectPlay;
		}
		break;
	}
	return state;
}

void MeetBoom(HDC hDC, Boom* head)
{
	addBoom(head, Bullet_DownRight, 0, 0, 50, 50);//1
	addBoom(head, Bullet_Down, 600, 0, 650, 50);//2
	addBoom(head, Bullet_DownLeft, 1150, 0, 1200, 50);//3
	addBoom(head, Bullet_Right, 0, 400, 50, 450);//4
	addBoom(head, Bullet_Left, 1150, 400, 1200, 450);//6
	addBoom(head, Bullet_UpRight, 0, 750, 50, 800);//7
	addBoom(head, Bullet_Up, 600, 750, 650, 800);//8
	addBoom(head, Bullet_UpLeft, 1150, 750, 1200, 800);//9
}

void SetUp(Boom* head, Boom* bullet_head)
{
	Tp = FALSE;
	Tp_2 = FALSE;

	Player_1.top = 380;
	Player_1.bottom = 405;
	Player_1.left = 380;
	Player_1.right = 405;
	PLAYER1_HIT = 0;

	Player_2.top = 380;
	Player_2.bottom = 405;
	Player_2.left = 410;
	Player_2.right = 435;
	PLAYER2_HIT = 0;

	Boom* p;
	p = head;
	while (p->nextBoom != NULL)
	{
		deleteBoom(p);
	}
	p = bullet_head;
	while (p->nextBoom != NULL)
	{
		deleteBoom(p);
	}
}