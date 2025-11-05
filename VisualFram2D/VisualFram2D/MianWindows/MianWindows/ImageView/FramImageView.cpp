#include <iostream>
#include "FramImageView.h"
#include "../StaticGlobalVariable.h"
#include "qmutex.h"

FramImageView::FramImageView(QWidget *parent)
    : QWidget(parent)
{	
    ui.setupUi(this);		
    InitialFrmImageView();
}

FramImageView::~FramImageView()
{
	this->deleteLater();
}


void FramImageView::SetFramImageLayout(FramImageLayout* frm_image_layout) {
    frmImageLayout = frm_image_layout;
    //¡¨Ω”–≈∫≈”Î≤€∫Ø ˝
    connect(frmImageLayout, &FramImageLayout::ImageShowModeSig, this, [this](int index) {
        SwitchImageShowMode(index);
        StaticGlobalVariable::ptrSystemSetParam->imageShowMode = index;
        });
    SwitchImageShowMode(StaticGlobalVariable::ptrSystemSetParam->imageShowMode);
}

void FramImageView::InitialFrmImageView()
{
	//≥ı ºªØÕºœÒœ‘ æΩÁ√Ê
	for (size_t i = 0; i < MAX_IMAGESHOW_VIEWS; i++) {
		ptrQGraphicsViews[i] = new QGraphicsViews(i);
	}

	hlay = new QHBoxLayout(this);
	//2ÕºœÒ	
	vlay2_1 = new QVBoxLayout();
	vlay2_2 = new QVBoxLayout();
	//3ÕºœÒ
	vlay3_1 = new QVBoxLayout();
	vlay3_2 = new QVBoxLayout();
	hlay3_2_1 = new QHBoxLayout();
	hlay3_2_2 = new QHBoxLayout();
	//4ÕºœÒ	
	vlay4 = new QVBoxLayout();
	hlay4_1 = new QHBoxLayout();
	hlay4_1_1 = new QHBoxLayout();
	hlay4_1_2 = new QHBoxLayout();
	hlay4_2 = new QHBoxLayout();
	hlay4_2_1 = new QHBoxLayout();
	hlay4_2_2 = new QHBoxLayout();
	//6ÕºœÒA	
	vlay6_1 = new QVBoxLayout();
	vlay6_1_1 = new QVBoxLayout();
	vlay6_1_2 = new QVBoxLayout();
	vlay6_2 = new QVBoxLayout();
	vlay6_2_1 = new QVBoxLayout();
	vlay6_2_2 = new QVBoxLayout();
	vlay6_3 = new QVBoxLayout();
	vlay6_3_1 = new QVBoxLayout();
	vlay6_3_2 = new QVBoxLayout();
	//6ÕºœÒB			
	vlay6B = new QVBoxLayout();
	hlay6B_1 = new QHBoxLayout();
	vlay6B_1_1 = new QVBoxLayout();
	vlay6B_1_2 = new QVBoxLayout();
	hlay6B_1_2_1 = new QHBoxLayout();
	hlay6B_1_2_2 = new QHBoxLayout();
	hlay6B_2 = new QHBoxLayout();
	vlay6B_2_1 = new QVBoxLayout();
	vlay6B_2_2 = new QVBoxLayout();
	vlay6B_2_3 = new QVBoxLayout();
	//9ÕºœÒ
	vlay9 = new QVBoxLayout();
	hlay9_1 = new QHBoxLayout();
	vlay9_1_1 = new QVBoxLayout();
	vlay9_1_2 = new QVBoxLayout();
	vlay9_1_3 = new QVBoxLayout();
	hlay9_2 = new QHBoxLayout();
	vlay9_2_1 = new QVBoxLayout();
	vlay9_2_2 = new QVBoxLayout();
	vlay9_2_3 = new QVBoxLayout();
	hlay9_3 = new QHBoxLayout();
	vlay9_3_1 = new QVBoxLayout();
	vlay9_3_2 = new QVBoxLayout();
	vlay9_3_3 = new QVBoxLayout();

	ImageShowSlot1();
}

void FramImageView::DispImage(const int& index, const QImage& qimage)
{
    ptrQGraphicsViews[index]->DispImage(qimage);
}

void FramImageView::SwitchImageShowMode(const int mode)
{
	std::cout << "mode: " << mode << std::endl;

	switch (mode)
	{
	case 0: {
		ImageShowSlot1();
		break;
	}
	case 1: {
		ImageShowSlot2();
		break;
	}
	case 2: {
		ImageShowSlot3();
		break;
	}
	case 3: {
		ImageShowSlot4();
		break;
	}
	case 4: {
		ImageShowSlot6A();
		break;
	}
	case 5: {
		ImageShowSlot6B();
		break;
	}
	case 6: {
		ImageShowSlot9();
		break;
	}
	default:
		break;
	}
}

void FramImageView::RemoveLayout()
{
	//1ÕºœÒ
	hlay->removeWidget(ptrQGraphicsViews[0]);
	//2ÕºœÒ
	vlay2_1->removeWidget(ptrQGraphicsViews[0]);
	hlay->removeItem(vlay2_1);
	vlay2_2->removeWidget(ptrQGraphicsViews[1]);
	hlay->removeItem(vlay2_2);
	//3ÕºœÒ
	vlay3_1->removeWidget(ptrQGraphicsViews[0]);
	hlay->removeItem(vlay3_1);
	hlay3_2_1->removeWidget(ptrQGraphicsViews[1]);
	vlay3_2->removeItem(hlay3_2_1);
	hlay3_2_2->removeWidget(ptrQGraphicsViews[2]);
	vlay3_2->removeItem(hlay3_2_2);
	hlay->removeItem(vlay3_2);
	//4ÕºœÒ
	hlay4_1_1->removeWidget(ptrQGraphicsViews[0]);
	hlay4_1->removeItem(hlay4_1_1);
	hlay4_1_2->removeWidget(ptrQGraphicsViews[1]);
	hlay4_1->removeItem(hlay4_1_2);
	vlay4->removeItem(hlay4_1);
	hlay4_2_1->removeWidget(ptrQGraphicsViews[2]);
	hlay4_2->removeItem(hlay4_2_1);
	hlay4_2_2->removeWidget(ptrQGraphicsViews[3]);
	hlay4_2->removeItem(hlay4_2_2);
	vlay4->removeItem(hlay4_2);
	hlay->removeItem(vlay4);
	//6ÕºœÒA
	vlay6_1_2->removeWidget(ptrQGraphicsViews[0]);
	vlay6_2_1->removeWidget(ptrQGraphicsViews[1]);
	vlay6_3_1->removeWidget(ptrQGraphicsViews[2]);
	vlay6_1_1->removeWidget(ptrQGraphicsViews[3]);
	vlay6_2_2->removeWidget(ptrQGraphicsViews[4]);
	vlay6_3_2->removeWidget(ptrQGraphicsViews[5]);
	vlay6_1->removeItem(vlay6_1_2);
	vlay6_1->removeItem(vlay6_1_1);
	vlay6_2->removeItem(vlay6_2_1);
	vlay6_2->removeItem(vlay6_2_2);
	vlay6_3->removeItem(vlay6_3_1);
	vlay6_3->removeItem(vlay6_3_2);
	hlay->removeItem(vlay6_1);
	hlay->removeItem(vlay6_2);
	hlay->removeItem(vlay6_3);
	//6ÕºœÒB
	vlay6B_1_1->removeWidget(ptrQGraphicsViews[0]);
	hlay6B_1->removeItem(vlay6B_1_1);
	hlay6B_1_2_1->removeWidget(ptrQGraphicsViews[1]);
	vlay6B_1_2->removeItem(hlay6B_1_2_1);
	hlay6B_1_2_2->removeWidget(ptrQGraphicsViews[2]);
	vlay6B_1_2->removeItem(hlay6B_1_2_2);
	hlay6B_1->removeItem(vlay6B_1_2);
	vlay6B->removeItem(hlay6B_1);
	vlay6B_2_1->removeWidget(ptrQGraphicsViews[3]);
	hlay6B_2->removeItem(vlay6B_2_1);
	vlay6B_2_2->removeWidget(ptrQGraphicsViews[4]);
	hlay6B_2->removeItem(vlay6B_2_2);
	vlay6B_2_3->removeWidget(ptrQGraphicsViews[5]);
	hlay6B_2->removeItem(vlay6B_2_3);
	vlay6B->removeItem(hlay6B_2);
	hlay->removeItem(vlay6B);
	//9ÕºœÒ
	vlay9_1_1->removeWidget(ptrQGraphicsViews[0]);
	hlay9_1->removeItem(vlay9_1_1);
	vlay9_1_2->removeWidget(ptrQGraphicsViews[1]);
	hlay9_1->removeItem(vlay9_1_2);
	vlay9_1_3->removeWidget(ptrQGraphicsViews[2]);
	hlay9_1->removeItem(vlay9_1_3);
	vlay9->removeItem(hlay9_1);
	vlay9_2_1->removeWidget(ptrQGraphicsViews[3]);
	hlay9_2->removeItem(vlay9_2_1);
	vlay9_2_2->removeWidget(ptrQGraphicsViews[4]);
	hlay9_2->removeItem(vlay9_2_2);
	vlay9_2_3->removeWidget(ptrQGraphicsViews[5]);
	hlay9_2->removeItem(vlay9_2_3);
	vlay9->removeItem(hlay9_2);
	vlay9_3_1->removeWidget(ptrQGraphicsViews[6]);
	hlay9_3->removeItem(vlay9_3_1);
	vlay9_3_2->removeWidget(ptrQGraphicsViews[7]);
	hlay9_3->removeItem(vlay9_3_2);
	vlay9_3_3->removeWidget(ptrQGraphicsViews[8]);
	hlay9_3->removeItem(vlay9_3_3);
	vlay9->removeItem(hlay9_3);
	hlay->removeItem(vlay9);

	for (size_t i = 0; i < MAX_IMAGESHOW_VIEWS; i++) {
		ptrQGraphicsViews[i]->setParent(nullptr);
	}
}

void FramImageView::ImageShowSlot1()
{
	RemoveLayout();
	hlay->setContentsMargins(0, 0, 0, 0);
	hlay->setSpacing(0);
	hlay->addWidget(ptrQGraphicsViews[0]);
}
void FramImageView::ImageShowSlot2()
{
	RemoveLayout();
	hlay->setContentsMargins(0, 0, 0, 0);
	hlay->setSpacing(0);
	vlay2_1->setSpacing(0);
	vlay2_1->addWidget(ptrQGraphicsViews[0]);
	vlay2_2->setSpacing(0);
	vlay2_2->addWidget(ptrQGraphicsViews[1]);
	hlay->addLayout(vlay2_1);
	hlay->addLayout(vlay2_2);
}

void FramImageView::ImageShowSlot3()
{
	RemoveLayout();
	hlay->setContentsMargins(0, 0, 0, 0);
	hlay->setSpacing(0);
	vlay3_1->setSpacing(0);
	vlay3_1->addWidget(ptrQGraphicsViews[0]);
	vlay3_2->setSpacing(0);
	hlay3_2_1->setSpacing(0);
	hlay3_2_1->addWidget(ptrQGraphicsViews[1]);
	hlay3_2_2->setSpacing(0);
	hlay3_2_2->addWidget(ptrQGraphicsViews[2]);
	vlay3_2->addLayout(hlay3_2_1);
	vlay3_2->addLayout(hlay3_2_2);
	hlay->addLayout(vlay3_1);
	hlay->addLayout(vlay3_2);
}

void FramImageView::ImageShowSlot4()
{
	RemoveLayout();
	hlay->setContentsMargins(0, 0, 0, 0);
	hlay->setSpacing(0);
	vlay4->setSpacing(0);
	hlay4_1->setSpacing(0);
	hlay4_1_1->setSpacing(0);
	hlay4_1_1->addWidget(ptrQGraphicsViews[0]);
	hlay4_1->addLayout(hlay4_1_1);
	hlay4_1_2->setSpacing(0);
	hlay4_1_2->addWidget(ptrQGraphicsViews[1]);
	hlay4_1->addLayout(hlay4_1_2);
	vlay4->addLayout(hlay4_1);
	hlay4_2->setSpacing(0);
	hlay4_2_1->setSpacing(0);
	hlay4_2_1->addWidget(ptrQGraphicsViews[2]);
	hlay4_2->addLayout(hlay4_2_1);
	hlay4_2_2->setSpacing(0);
	hlay4_2_2->addWidget(ptrQGraphicsViews[3]);
	hlay4_2->addLayout(hlay4_2_2);
	vlay4->addLayout(hlay4_2);
	hlay->addLayout(vlay4);
}

void FramImageView::ImageShowSlot6A()
{
	RemoveLayout();
	hlay->setContentsMargins(0, 0, 0, 0);
	hlay->setSpacing(0);
	vlay6_1->setSpacing(0);
	vlay6_1_1->setSpacing(0);
	vlay6_1_2->setSpacing(0);
	vlay6_2->setSpacing(0);
	vlay6_2_1->setSpacing(0);
	vlay6_2_2->setSpacing(0);
	vlay6_3->setSpacing(0);
	vlay6_3_1->setSpacing(0);
	vlay6_3_2->setSpacing(0);
	vlay6_1_2->addWidget(ptrQGraphicsViews[0]);
	vlay6_2_1->addWidget(ptrQGraphicsViews[1]);
	vlay6_3_1->addWidget(ptrQGraphicsViews[2]);
	vlay6_1_1->addWidget(ptrQGraphicsViews[3]);
	vlay6_2_2->addWidget(ptrQGraphicsViews[4]);
	vlay6_3_2->addWidget(ptrQGraphicsViews[5]);
	vlay6_1->addLayout(vlay6_1_2);
	vlay6_1->addLayout(vlay6_1_1);
	vlay6_2->addLayout(vlay6_2_1);
	vlay6_2->addLayout(vlay6_2_2);
	vlay6_3->addLayout(vlay6_3_1);
	vlay6_3->addLayout(vlay6_3_2);
	hlay->addLayout(vlay6_1);
	hlay->addLayout(vlay6_2);
	hlay->addLayout(vlay6_3);
}

void FramImageView::ImageShowSlot6B()
{
	RemoveLayout();
	hlay->setContentsMargins(0, 0, 0, 0);
	hlay->setSpacing(0);
	vlay6B->setSpacing(0);
	hlay6B_1->setSpacing(0);
	vlay6B_1_1->setSpacing(0);
	vlay6B_1_1->addWidget(ptrQGraphicsViews[0]);
	hlay6B_1->addLayout(vlay6B_1_1);
	vlay6B_1_2->setSpacing(0);
	hlay6B_1_2_1->setSpacing(0);
	hlay6B_1_2_1->addWidget(ptrQGraphicsViews[1]);
	vlay6B_1_2->addLayout(hlay6B_1_2_1);
	hlay6B_1_2_2->setSpacing(0);
	hlay6B_1_2_2->addWidget(ptrQGraphicsViews[2]);
	vlay6B_1_2->addLayout(hlay6B_1_2_2);
	hlay6B_1->addLayout(vlay6B_1_2);
	hlay6B_1->setStretchFactor(vlay6B_1_1, 2);
	hlay6B_1->setStretchFactor(vlay6B_1_2, 1);
	vlay6B->addLayout(hlay6B_1);
	hlay6B_2->setSpacing(0);
	vlay6B_2_1->setSpacing(0);
	vlay6B_2_1->addWidget(ptrQGraphicsViews[3]);
	hlay6B_2->addLayout(vlay6B_2_1);
	vlay6B_2_2->setSpacing(0);
	vlay6B_2_2->addWidget(ptrQGraphicsViews[4]);
	hlay6B_2->addLayout(vlay6B_2_2);
	vlay6B_2_3->setSpacing(0);
	vlay6B_2_3->addWidget(ptrQGraphicsViews[5]);
	hlay6B_2->addLayout(vlay6B_2_3);
	vlay6B->addLayout(hlay6B_2);
	vlay6B->setStretchFactor(hlay6B_1, 2);
	vlay6B->setStretchFactor(hlay6B_2, 1);
	hlay->addLayout(vlay6B);
}

void FramImageView::ImageShowSlot9()
{
	RemoveLayout();
	hlay->setContentsMargins(0, 0, 0, 0);
	hlay->setSpacing(0);
	vlay9->setSpacing(0);
	hlay9_1->setSpacing(0);
	vlay9_1_1->setSpacing(0);
	vlay9_1_1->addWidget(ptrQGraphicsViews[0]);
	hlay9_1->addLayout(vlay9_1_1);
	vlay9_1_2->setSpacing(0);
	vlay9_1_2->addWidget(ptrQGraphicsViews[1]);
	hlay9_1->addLayout(vlay9_1_2);
	vlay9_1_3->setSpacing(0);
	vlay9_1_3->addWidget(ptrQGraphicsViews[2]);
	hlay9_1->addLayout(vlay9_1_3);
	vlay9->addLayout(hlay9_1);
	hlay9_2->setSpacing(0);
	vlay9_2_1->setSpacing(0);
	vlay9_2_1->addWidget(ptrQGraphicsViews[3]);
	hlay9_2->addLayout(vlay9_2_1);
	vlay9_2_2->setSpacing(0);
	vlay9_2_2->addWidget(ptrQGraphicsViews[4]);
	hlay9_2->addLayout(vlay9_2_2);
	vlay9_2_3->setSpacing(0);
	vlay9_2_3->addWidget(ptrQGraphicsViews[5]);
	hlay9_2->addLayout(vlay9_2_3);
	vlay9->addLayout(hlay9_2);
	hlay9_3->setSpacing(0);
	vlay9_3_1->setSpacing(0);
	vlay9_3_1->addWidget(ptrQGraphicsViews[6]);
	hlay9_3->addLayout(vlay9_3_1);
	vlay9_3_2->setSpacing(0);
	vlay9_3_2->addWidget(ptrQGraphicsViews[7]);
	hlay9_3->addLayout(vlay9_3_2);
	vlay9_3_3->setSpacing(0);
	vlay9_3_3->addWidget(ptrQGraphicsViews[8]);
	hlay9_3->addLayout(vlay9_3_3);
	vlay9->addLayout(hlay9_3);
	hlay->addLayout(vlay9);
}