#ifndef LENGTHCHANGINGGENE_H
#define LENGTHCHANGINGGENE_H

#include <qnn-global.h>

#include "genericgene.h"

class QNNSHARED_EXPORT LengthChangingGene : public GenericGene
{
public:
    struct config {
        int min_length; // -1 = initialLength
        int max_length; // -1 = initialLength*4

        config() :
            min_length(-1),
            max_length(-1)
        {
        }
    };

    LengthChangingGene(int initialLength, int segment_size = 1, config config = config());
    ~LengthChangingGene();

    void mutate();
    GenericGene *createCopy();

    static GenericGene *loadThisGene(QIODevice *device);

protected:
    LengthChangingGene();
    LengthChangingGene(QList< QList<int> > gene, int segment_size, config config = config());

    GenericGene *createGene(QList< QList<int> > gene, int segment_size);

    QString identifier();
    bool _saveGene(QTextStream *stream);
    GenericGene *_loadGene(QList< QList<int> > gene, int segment_size, QTextStream *stream);

    config _config;
};

#endif // LENGTHCHANGINGGENE_H
