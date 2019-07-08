#ifndef RANGE_H
#define RANGE_H

#include <QList>
#include <QPair>
#include <QtAlgorithms>

template<typename T>
class Range {
public:
    typedef QPair<T, T> Segment;
    typedef QList<Segment> Segments;
    Range() = default;
    Range& operator+=(const Segment& segment) {
        addSegment(segment);
        return *this;
    }

    const Segments& getSegments() const {
        return segments;
    }

    T bytesAvailable() {
        return segments.isEmpty() || segments.at(0).first != 0?
                0:segments.at(0).second - segments.at(0).first;
    }
private:
    Segments segments;
    void addSegment(const Segment& segment) {
        Q_ASSERT(segment.second > segment.first);
        typename Segments::iterator ins = qLowerBound(segments.begin(), segments.end(), segment);

        if (ins != segments.end() && (ins->first == segment.second)) {
            ins->first = segment.first;
        } else {
            ins = segments.insert(ins, segment);
        }

        if (ins != segments.begin()) {
            --ins;
            if (ins->second == segment.first) {
                T first = ins->first;
                typename Segments::iterator del = segments.erase(ins);
                del->first = first;
            }
        }
    }
};

#endif // RANGE_H
