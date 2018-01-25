
#include "statistic.h"


void resetStatistic(Statistic_ptr stat, int dataCount){
	stat->dataCount = dataCount;
	stat->min = INFINITY;
	stat->max = -INFINITY;
	stat->median = 0;
	stat->avg = 0;
	stat->std = 0;
}

void updateStatistic(Statistic_ptr stat, float value, int index){
	if(value < stat->min){
		stat->min = value;
	}

	if(value > stat->max){
		stat->max = value;
	}

	if(stat->dataCount % 2 == 1){
		if(index == stat->dataCount / 2){
			stat->median = value;
		}
	}
	else{
		if(index == stat->dataCount / 2 || index == stat->dataCount / 2 - 1 ){
			stat->median += value;
		}
	}

	stat->avg += value;

	stat->std += value * value;

}

void finalizeStatistic(Statistic_ptr stat){
	if(stat->dataCount % 2 == 0){
		stat->median /= 2;
	}

	stat->avg /= stat->dataCount;

	stat->std = sqrt(stat->std / stat->dataCount - stat->avg * stat->avg);

}

void printStatisticCsvHeader(const char* statName, FILE* file){
	fprintf(file, "%sMin, %sMax, %sMedian, %sAvg, %sStd,", statName, statName, statName, statName, statName);
}

void printStatisticCsvData(Statistic_ptr stat, FILE* file){
	fprintf(file, "%f,%f,%f,%f,%f,", stat->min, stat->max, stat->median, stat->avg, stat->std);
}
