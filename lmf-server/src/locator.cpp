// my_class.cpp
#include "locator.h" // header in local directory
#include <iostream> // header in standard library
#include <sys/resource.h>

using namespace loc;



locator::locator(){
    n=3;
    distances=arma::colvec(3, arma::fill::zeros); //column with distance? for each anchor; 
    anc_pos=arma::mat(3,2,arma::fill::zeros); // one row for each anchor to store (x,y) values
    start_pos=arma::colvec(3, arma::fill::zeros);
    est_pos_corrent=arma::colvec(3, arma::fill::zeros);
}

arma::mat locator::localize(arma::mat distances){
    this->distances=distances;
    if (!start_pos.is_finite()) {
      std::cout << "LLS estimation out of bounds. Aborting estimation.\n";
    }

    // nLLS phase
    try {
        est_pos_corrent = trilateration_nlls(n,start_pos,W);
        return est_pos_corrent;
    } catch (...) {
        std::cout << "Matrix is not invertible. Aborting estimation.\n";
        return arma::mat(0,0, arma::fill::zeros);
    }
    // predicted_error();
}

void locator::first_lls_init(arma::mat distances, arma::mat anc_pos){
    cout<<"first_lls_init"<<endl;
    this->distances=distances;
    this->anc_pos=anc_pos;
    W = arma::diagmat(1/(1+distances)); // weighting matrix
    // if first time => use LLS
    bool always_lls_init = true; //It was initializated to true
    if (always_lls_init || est_pos_corrent.is_empty()) {
      try {
        start_pos = trilateration_lls(n,W);
      } catch (...) {
        std::cout << "Matrix is not invertible.";
        if (est_pos_corrent.is_empty()) {
          std::cout << " Selecting (0,0) as starting point.\n";
          start_pos(0) = 0;
          start_pos(1) = 0;
        }  else {
          start_pos = est_pos_corrent;
        }
      }
    }
}


inline arma::mat locator::solve_normal_equation(arma::mat A, arma::mat b, arma::mat W) {
  return arma::solve(A.t()*W*A, A.t()*W*b);
}

arma::mat locator::trilateration_lls(int N, arma::mat W) {

  // calc A MATLAB: A = anc_pos - repmat(mean(pos),N,1);
  arma::mat A = anc_pos - arma::repmat(arma::mean(anc_pos),N,1);
  // calc b MATLAB: b = 0.5*(sum(anc_pos.^2,2) - sum(mean(anc_pos.^2)) - dist.^2 + mean(dist.^2));
  arma::mat anc_pos_squared = arma::square(anc_pos); // pos.^2
  arma::mat dist_squared = arma::square(distances); // dist.^2
  arma::mat b = 0.5*(arma::sum(anc_pos_squared,1) - arma::as_scalar(arma::sum(arma::mean(anc_pos_squared),1))
      - dist_squared + arma::as_scalar(arma::mean(dist_squared)));
  // calc pos
  return solve_normal_equation(A,b,W);
}


arma::mat locator::trilateration_nlls(int N,arma::mat start,arma::mat W) {
  // parameters
  double min_norm = 1e-3;
  int max_it = 500;
  double frac = 0.1;

  // start_value
  arma::mat est_pos_corrent = start;
  arma::mat old_pos = est_pos_corrent;

  // loop
  bool stop = false;
  int it = 0;
  while (!stop) {
    //calc f and J
    arma::mat f(N,1);
    arma::mat J(N,2);
    for (int i = 0;i<N;i++) {
      // MATLAB: temp_val = sqrt((est(1)-pos(i,1))^2 + (est(2)-pos(i,2))^2);
      double temp_val = sqrt(pow(est_pos_corrent(0)-anc_pos(i,0),2) + pow(est_pos_corrent(1)-anc_pos(i,1),2));
      f(i,0) = temp_val - distances(i);
      J(i,0) = (est_pos_corrent(0) - anc_pos(i,0)) / temp_val;
      J(i,1) = (est_pos_corrent(1) - anc_pos(i,1)) / temp_val;
    }

    // calculate new position
    arma::mat delta_est = solve_normal_equation(J,f,W);
    est_pos_corrent = est_pos_corrent - frac*delta_est;

    // stopping conditions
    if(!est_pos_corrent.is_finite()) {
      stop = true;
      est_pos_corrent = start;
      std::cout << "Convergence not reached. Divergence found, using stating reference." << std::endl;
    }else if (arma::norm(delta_est,2) < min_norm) {
      stop = true;
      std::cout << "Convergence reached after " << it << " iterations." << std::endl;
    }else if (++it == max_it) {
      stop = true;
      std::cout << "Convergence not reached." << std::endl;
    }
  }

  //std::cout << "Used " << it << " iteration(s)\n";
  return est_pos_corrent;

}


double locator::predicted_error(){
    // predicted error
    std::cout << "Per anchor error: " ;
    double pred_err = 0;
    for(unsigned int i = 0; i<distances.n_rows; i++) {
      double res = arma::norm(est_pos_corrent-anc_pos.row(i).t(),2) - distances(i);
      if (distances(i) > -0.01) {
        std::cout << floor(res) << " ";
        pred_err += pow(res,2);
      } else
        std::cout << "invalid ";
    }
    std::cout << std::endl;
    pred_err = sqrt(pred_err)/distances.n_rows;
    return pred_err;
}
