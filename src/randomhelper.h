#ifndef RANDOMHELPER_H
#define RANDOMHELPER_H

#include <qnn-global.h>

#include <random>
#include <QVector>
#include <QThread>
#include <QTime>
#include <QDate>

namespace RandomHelper {
/*!
 * \brief Random engine
 */
static thread_local std::mt19937 rnd;

/*!
 * \brief Shows wether the random engine has been initialised
 */
static thread_local bool initialised = false;

/*!
 * \brief Initialises the engine
 *
 * This method checks wether the random engine has been initialised and initialises the random engine if it has not been initialised
 */
inline void check_initialised()
{
    if(Q_UNLIKELY(!initialised))
    {
        QNN_WARNING_MSG("Random initialised");
        std::random_device random_device;
        QVector<int> seed_vector;
        seed_vector.reserve(4);
        seed_vector << random_device();
        seed_vector << QTime(0,0,0).msecsTo(QTime::currentTime());
        seed_vector << QDate::currentDate().dayOfYear();
        seed_vector << QDate::currentDate().year();
        std::seed_seq seed(seed_vector.begin(), seed_vector.end());
        rnd.seed(seed);
        initialised = true;
    }
}

/*!
 * \brief Returns a random uniform distributed integer in the range [min,max]
 * \param min Minimum
 * \param max Maximum
 * \return Random integer
 */
inline qint32 getRandomInt(qint32 min, qint32 max)
{
    check_initialised();
    std::uniform_int_distribution<qint32> distribution(min, max);
    return distribution(rnd);
}

/*!
 * \brief Returns a random uniform distributed double in the range [min,max)
 * \param min Minimum
 * \param max Maximum
 * \return Random double
 */
inline double getRandomDouble(double min, double max)
{
    check_initialised();
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(rnd);
}

/*!
 * \brief Returns a random normal distributed double
 * \return Random double
 */
inline double getNormalDistributedDouble()
{
    check_initialised();
    std::normal_distribution<double> distribution;
    return distribution(rnd);
}

/*!
 * \brief Returns a random bool
 * \return Random bool
 */
inline bool getRandomBool()
{
    check_initialised();
    std::uniform_int_distribution<qint32> distribution(0, 1);
    return distribution(rnd);
}
}

#endif // RANDOMHELPER_H
