#pragma once

extern "C"
{
#include "../dip/dip_funcs.h"
}
#include <map>

#include <ConstraintLayout.h>
#include <Image.h>

class ImageArea :
public d14uikit::ConstraintLayout
{
    d14uikit::Image m_currImg{0, 0};

    double m_ratio = 1.0;

    struct BitmapPair
    {
        Bitmap raw, scaled;
    };
    using StepBmpMap =
    std::map<std::wstring, BitmapPair>;

    StepBmpMap m_stepBmps = {};

    int m_currBmpCode = {};
    std::wstring m_currBmpName = {};

    void updateImgView();
    void onRemoveAllBmp();

public:
    ImageArea();

    double ratio() const;
    void setRatio(double c);

    void updateScaledBmp();

    Bitmap currBmp() const;
    Bitmap currScaledBmp() const;
    int currBmpCode() const;
    const std::wstring& currBmpName() const;

    void addBmp(const std::wstring& opStr, Bitmap bmp);
    void selectBmp(const std::wstring& name);
    void removeBmp(const std::wstring& name);
    void removeAllBmps();

    bool colorLock = false;

    enum class ScaleMode
    {
        Nearest, Bilinear, Cubic
    }
    scaleMode = ScaleMode::Nearest;
};
