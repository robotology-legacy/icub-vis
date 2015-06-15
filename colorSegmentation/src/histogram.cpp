
// Copyright: 2003 Lorenzo Natale
// Author: Lorenzo Natale
// CopyPolicy: Released under the terms of the GNU GPL v2.0. (2010)
// CopyPolicy: Academic Free License Version 10 (2003)

#include <iCub/vis/histogram.h>
#include <stdio.h>

////////////////////
// Histo3D class

Histo3D::Histo3D()
{
	_nElem = 0;
	_lut = NULL;
}

Histo3D::~Histo3D()
{
	if (_lut != NULL)
		delete [] _lut;
}

void Histo3D::clean()
{
	unsigned int i;
	_maximum = 1.0;
	
	if (_lut != NULL)
	{
		for(i = 0; i < _nElem; i++)
			_lut[i].reset();
	}
}

void Histo3D::resize(unsigned char max, unsigned char min, unsigned char size)
{
	unsigned char tmpMax[3];
	unsigned char tmpMin[3];
	unsigned char tmpSize[3];

	int i;
	for (i = 0; i < 3; i++)
	{
		tmpMax[i] = max;
		tmpMin[i] = min;
		tmpSize[i] = size;
	}

	resize(tmpMax, tmpMin, tmpSize);
}

void Histo3D::resize(unsigned char max, unsigned char min, unsigned char *size)
{ 
	unsigned char tmpMax[3];
	unsigned char tmpMin[3];
		
	int i;
	for (i = 0; i < 3; i++)
	{
		tmpMax[i] = max;
		tmpMin[i] = min;
	}

	resize(tmpMax, tmpMin, size);
}

void Histo3D::resize(unsigned char *max, unsigned char *min, unsigned char *size)
{
	for(int i = 0; i < 3; i++)
	{
		_max[i] = max[i];
		_min[i] = min[i];
		_size[i] = size[i];
		_delta[i] = (_max[i]+1-_min[i])/(float) _size[i];

		/*if (((_max[i]-_min[i])%_size[i]) != 0)
			_delta[i] += 1;*/
		
	}

	_nElem = _size[0]*_size[1]*_size[2];
	if (_lut != NULL)
		delete [] _lut;

	_lut = new HistoEntry[_nElem];

	printf("Elementi: %d %lf %lf %lf\n", _nElem, _delta[0], _delta[1], _delta[2]);
}

/////////////
// Histo1D
Histo1D::Histo1D()
{
	_nElem = 0;
	_lut = NULL;
}

void Histo1D:: resize(unsigned char max, unsigned char min, unsigned char size)
{
	_max = max;
	_min = min;
	_size = size;
	_delta = (unsigned char) ((_max-_min)/_size);
	if (((_max-_min)%_size) != 0)
		_delta += 1;
		
	if (_lut != NULL)
		delete [] _lut;

	_nElem = _size;

	_lut = new HistoEntry[_nElem];
}

void Histo1D::clean()
{
	_maximum = 1.0;
		
	if (_lut != NULL)
	{
		unsigned int i;
		for(i = 0; i < _nElem; i ++)
			_lut[i].reset();
	}
}

////////////////
// Histogram class

Histogram::Histogram()
{
	clean();
}

Histogram::Histogram(unsigned char max, unsigned char min, unsigned char n)
{
	clean();

	Resize(max, min, n);
}

Histogram::Histogram(unsigned char max, unsigned char min, unsigned char *n)
{
	clean();

	Resize(max, min, n);
}

int Histogram::_dumpFull(const char *file)
{
	FILE *fp;
	fp = fopen(file, "w");

	fprintf(fp, "%lf\n", _3dlut._maximum);
	int i;
	for(i = 0; i<3; i++)
		fprintf(fp, "%d %d %d\n", _3dlut._max[i], _3dlut._min[i], _3dlut._size[i]);

	HistoEntry tmpEntry;
	HistoKey tmpKey;
			
	unsigned int it = _3dlut.begin();

	while  (_3dlut.find(it, tmpEntry)!=-1)
	{
		if (tmpKey.b == _3dlut._size[2])
		{
			tmpKey.b = 0;
			tmpKey.g++;
			if (tmpKey.g == _3dlut._size[1])
			{
				tmpKey.g = 0;
				tmpKey.r++;
			}
		}

		fprintf(fp, "%d %d %d %lf\n", tmpKey.r, tmpKey.g, tmpKey.b, tmpEntry.value());
		it++;
		tmpKey.b++;
	}

	fclose(fp);

	return 1;
}

int Histogram::_load1D(const char *file, Histo1D &lut)
{
	FILE *fp;
	fp = fopen(file, "rt");

	if (fp == NULL)
		return 0;

	int tmpMax;
	int tmpMin;
	int tmpSize;
	double tmpMaximum;
    size_t ret;

	ret=fscanf(fp, "%lf\n", &tmpMaximum);
    if (ret!=1)
        return 0;

	ret=fscanf(fp, "%d %d %d\n", &tmpMax, &tmpMin, &tmpSize);
    if (ret!=3)
        return 0;

	lut.resize((unsigned char) tmpMax, (unsigned char) tmpMin, (unsigned char) tmpSize);
	lut._maximum = tmpMaximum;

	HistoEntry *tmpEntry;
				
	unsigned int it = lut.begin();

	while  (lut.find(it, &tmpEntry)!=-1)
	{
		int tmp;
		double v;
		ret=fscanf(fp, "%d \t%lf\n", &tmp, &v);
		tmpEntry->setValue(v);
		it++;
	}

	fclose(fp);

	return 1;
}

int Histogram::_loadFull(const char *file)
{
	FILE *fp;
	fp = fopen(file, "rt");
	if (fp == NULL)
		return 0;

	double tmpMaximum;
	unsigned char max[3];
	unsigned char min[3];
	unsigned char size[3];
	int tmpMax;
	int tmpMin;
	int tmpSize;
	size_t ret;

	ret=fscanf(fp, "%lf\n", &tmpMaximum);
    if (ret!=1)
        return 0;
	
	int i;
	for(i = 0; i<3; i++)
	{
		ret=fscanf(fp, "%d %d %d\n", &tmpMax, &tmpMin, &tmpSize);
        if (ret!=3)
            return 0;

		max[i] = (unsigned char) tmpMax;
		min[i] = (unsigned char) tmpMin;
		size[i] = (unsigned char) tmpSize;
	}

	_3dlut.resize(max, min, size);
	_3dlut._maximum = tmpMaximum;

	HistoEntry *tmpEntry;
	int dummy1;
	int dummy2;
	int dummy3;
			
	unsigned int it = _3dlut.begin();

	while  (_3dlut.find(it, &tmpEntry)!=-1)
	{
		double v;
		ret=fscanf(fp, "%d %d %d %lf\n", &dummy1, &dummy2, &dummy3, &v);
        if (ret!=4)
            return 0;

		tmpEntry->setValue(v);
		it++;
	}

	fclose(fp);
	
	return 1;
}

void Histogram::Resize(unsigned char max, unsigned char min, unsigned char n)
{
   	clean();
	
	_3dlut.resize(max, min, n);

	_rlut.resize(max, min, n);
	_glut.resize(max, min, n);
	_blut.resize(max, min, n);
}

void Histogram::Resize(unsigned char max, unsigned char min, unsigned char *n)
{
	clean();
		
	_3dlut.resize(max, min, n);
	_rlut.resize(max, min, n[0]);
	_glut.resize(max, min, n[1]);
	_blut.resize(max, min, n[2]);
}

int Histogram::dump(const char *basename)
{
//	_dumpFull((basename+".full").c_str());
//	_dump1D((basename+".r").c_str(), _rlut);
//	_dump1D((basename+".g").c_str(), _glut);
//	_dump1D((basename+".b").c_str(), _blut);

	return 1;
}

int Histogram::load(const char *basename)
{
//	_loadFull((basename+".full").c_str());
//	_load1D((basename+".r").c_str(), _rlut);
//	_load1D((basename+".g").c_str(), _glut);
//	_load1D((basename+".b").c_str(), _blut);

	return 1;
}

int Histogram::_dump1D(const char *file, Histo1D &lut)
{
	FILE *fp;
	fp = fopen(file, "w");

	fprintf(fp, "%lf\n", lut._maximum);
	fprintf(fp, "%d %d %d\n", lut._max, lut._min, lut._size);

	HistoEntry tmpEntry;
				
	unsigned int it = lut.begin();

	while  (lut.find(it, tmpEntry)!=-1)
	{
		fprintf(fp, "%d \t%lf\n", it, tmpEntry.value());
		it++;
	}

	fclose(fp);

	return 1;
}

void Histogram::clean()
{
	_3dlut.clean();
	_rlut.clean();
	_glut.clean();
	_blut.clean();
}

void Histogram::Apply(unsigned char r, unsigned char g, unsigned char b, double w)
{
	HistoEntry *tmpEntryP = NULL;
	unsigned int it;

	/////////////////// 3d histo
	_3dlut.pixelToKey(r, g, b, &it);

	if (_3dlut.find(it, &tmpEntryP) != -1)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _3dlut._maximum)
			_3dlut._maximum = tmpMax;
	}

	/////////////////// r histo
	_rlut.pixelToKey(r, &it);
	if (_rlut.find(it, &tmpEntryP) != -1)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _rlut._maximum)
			_rlut._maximum = tmpMax;
	}
	
	/////////////////// g histo
	_glut.pixelToKey(g, &it);
	if (_glut.find(it, &tmpEntryP)!=-1)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _glut._maximum)
			_glut._maximum = tmpMax;
	}
			
	/////////////////// b histo
	_blut.pixelToKey(b, &it);
	if (_blut.find(it, &tmpEntryP) != -1)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _blut._maximum)
			_blut._maximum = tmpMax;
	}

}
