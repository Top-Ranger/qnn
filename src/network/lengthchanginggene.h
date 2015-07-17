#ifndef LENGTHCHANGINGGENE_H
#define LENGTHCHANGINGGENE_H

#include <qnn-global.h>

#include "genericgene.h"

class QNNSHARED_EXPORT LengthChangingGene : public GenericGene
{
public:
    struct LengthChangingGene_config {
        int min_length; // -1 = initialLength
        int max_length; // -1 = initialLength*4

        LengthChangingGene_config() :
            min_length(-1),
            max_length(-1)
        {
        }
    };

    LengthChangingGene(int initialLength, int segment_size = 1, LengthChangingGene_config config = LengthChangingGene_config());
    ~LengthChangingGene();

    void mutate();

    static GenericGene *loadThisGene(QIODevice *device);

protected:
    LengthChangingGene();
    LengthChangingGene(QList< QList<int> > gene, int segment_size, LengthChangingGene_config config = LengthChangingGene_config());

    GenericGene *createGene(QList< QList<int> > gene, int segment_size);

    QString identifier();
    bool _saveGene(QTextStream *stream);
    GenericGene *_loadGene(QList< QList<int> > gene, int segment_size, QTextStream *stream);

    LengthChangingGene_config _config;
};

#endif // LENGTHCHANGINGGENE_H
