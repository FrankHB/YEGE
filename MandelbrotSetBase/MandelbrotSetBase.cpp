//���Ŵ�mandelbrot����ʾ
#include <complex>
#include <graphics.h>

constexpr std::size_t ITERATIONS(300U);     // ����������Խ�ߣ�ͼ��Խ��ϸ
constexpr std::size_t MAXCOLOR(200U);     // ��ɫ����Խ��ͼ��Խƽ�����������ڵ�������

int Color[MAXCOLOR];

void InitColor(float h1 = 240, float h2 = 320)
{
    for (auto i = 0; i < MAXCOLOR / 2; ++i)
    {
        Color[i] = HSLtoRGB(h1, 1.0F, i * 2.0F / MAXCOLOR);
        Color[MAXCOLOR - i - 1] = HSLtoRGB(h2, 1.0F, i * 2.0F / MAXCOLOR);
    }
}

template<typename T>
inline T
norm(const std::complex<T>& z)
{
	using namespace std;

    if(isinf(z.real()))
        return abs(z.real());
    if(isinf(z.imag()))
        return abs(z.imag());
    return z.real() * z.real() + z.imag() * z.imag();
}

std::size_t
f(std::complex<double> c)
{
    std::complex<double> z(c);
    auto maxcalc(ITERATIONS - 1);

//	while(!(std::norm(z) > 4.0) && --maxcalc)
	while(--maxcalc && ::norm(z) < 4.0)
        z = z * z + c;
	return maxcalc;
}



void Draw(double fromx, double fromy, double tox, double toy)
{
    std::complex<double> z, c;

	for(std::size_t x(0); x < 640; ++x)
    {
        c.real(fromx + (tox - fromx) * (x / 640.0));
        for(std::size_t y(0); y < 480; ++y)
        {
            c.imag(fromy + (toy - fromy) * (y / 480.0));

			auto k = f(c);

            putpixel(x, y, k > 0 ? k = Color[(ITERATIONS - k) % MAXCOLOR] : 0);
        }
    }
}


/////////////////////////////////////////////////
// ������
/////////////////////////////////////////////////
int main()
{
    initgraph(640, 480);
    InitColor();

	double fromx(-2.2), fromy(-1.65), tox(2.2), toy(1.65);

    Draw(fromx, fromy, tox, toy);
    {
        bool isLDown = false;
        int selfx, selfy, seltx, selty; // ����ѡ��

        while (kbhit() != -1)
        {
            mouse_msg m(getmouse());
            switch (m.msg)
            {
                // ������Ҽ��ָ�ԭͼ������ϵ
            case WM_RBUTTONUP:
                fromx = -2.2; tox = 1.2;
                fromy = -1.65; toy = 1.65;
                Draw(fromx, fromy, tox, toy);
                break;

                // �����������϶���ѡ������
            case WM_MOUSEMOVE:
                if (isLDown)
                {
                    rectangle(selfx, selfy, seltx, selty);
                    seltx = m.x;
                    selty = m.y;
                    rectangle(selfx, selfy, seltx, selty);
                }
                break;

                // �����������϶���ѡ������
            case WM_LBUTTONDOWN:
                setcolor(WHITE);
                setwritemode(R2_XORPEN);
                isLDown = true;
                selfx = seltx = m.x;
                selfy = selty = m.y;
                rectangle(selfx, selfy, seltx, selty);

                break;

                // �����������϶���ѡ������
            case WM_LBUTTONUP:
                rectangle(selfx, selfy, seltx, selty);
                setwritemode(R2_COPYPEN);
                isLDown = false;
                seltx = m.x;
                selty = m.y;

                if (selfx == seltx || selfy == selty) break;

                // ����ѡ��Ϊ 4:3
                {
                    int tmp;
                    if (selfx > seltx)
                    {
                        tmp = selfx; selfx = seltx; seltx = tmp;
                    }
                    if (selfy > selty)
                    {
                        tmp = selfy; selfy = selty; selty = tmp;
                    }
                }

                if ( (seltx - selfx) * 0.75 < (selty - selfy) )
                {
                    selty += (3 - (selty - selfy) % 3);
                    selfx -= (selty - selfy) / 3 * 4 / 2
                             - (seltx - selfx) / 2;
                    seltx = selfx + (selty - selfy) / 3 * 4;
                }
                else
                {
                    seltx += (4 - (seltx - selfx) % 4);
                    selfy -= (seltx - selfx) * 3 / 4 / 2
                             - (selty - selfy ) / 2;
                    selty = selfy + (seltx - selfx ) * 3 / 4;
                }

                // ��������ϵ
                {
                    double f, t;
                    f = fromx + (tox - fromx) * selfx / 640;
                    t = fromx + (tox - fromx) * seltx / 640;
                    fromx = f;
                    tox = t;
                    f = fromy + (toy - fromy) * selfy / 480;
                    t = fromy + (toy - fromy) * selty / 480;
                    fromy = f;
                    toy = t;
                }

                // ��ͼ��
                Draw(fromx, fromy, tox, toy);
                break;
            }
        }
    }

    getch();
    closegraph();
    return 0;
}

