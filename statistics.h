#ifndef STATISTICS_H
#define STATISTICS_H

void statistics_print_array(int n, const int v[]);

double statistics_average(int n, const int v[]);

/*Pass NULL to avg if you did not precalculate it*/
double statistics_stddev(int n, const int v[], const double* avg);

void statistics_print(int n, const int v[]);

#endif /*STATISTICS_H*/
