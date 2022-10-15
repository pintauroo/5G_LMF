// locator.h
#ifndef LOCATOR_H // include guard
#define LOCATOR_H

#include <armadillo>
using namespace std;
using namespace arma;


namespace loc
{
    class locator
    {
        private:
            int n; 
            arma::mat distances;
            arma::mat anc_pos;
            arma::mat start_pos;
            arma::mat est_pos_corrent;
            arma::mat W;         

		    inline arma::mat solve_normal_equation(arma::mat A, arma::mat b, arma::mat W);
		    arma::mat trilateration_lls(int n, arma::mat W);
    		arma::mat trilateration_nlls(int n,arma::mat start, arma::mat W);
            double predicted_error();
        public:
            locator();
            arma::mat localize(arma::mat distances);
            void first_lls_init(arma::mat distances, arma::mat anc_pos);

    };
}

#endif /* LOCATOR_H */
