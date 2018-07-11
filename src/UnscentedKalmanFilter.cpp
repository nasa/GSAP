/**  UnscentedKalmanFilter - Body
 *   @file       UnscentedKalmanFilter.cpp
 *   @ingroup    GPIC++
 *   @defgroup   Observer     Observer
 *
 *   @brief      Unscented Kalman filter class. Implements UKF state estimation algorithm for nonlineary models.
 *               Uses the Model class.
 *
 *   @author     Matthew Daigle
 *   @version    0.1.0
 *
 *   @pre        N/A
 *
 *      Contact: Matthew Daigle (matthew.j.daigle@nasa.gov)
 *      Created: March 5, 2016
 *
 *   @copyright Copyright (c) 2018 United States Government as represented by
 *     the Administrator of the National Aeronautics and Space Administration.
 *     All Rights Reserved.
 */

#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

#include "GSAPConfigMap.h"
#include "Model.h"
#include "UData.h"

#include "Exceptions.h"
#include "UnscentedKalmanFilter.h"

namespace PCOE {
    // Configuration Keys
    const std::string Q_KEY = "Observer.Q";
    const std::string R_KEY = "Observer.R";
    const std::string K_KEY = "Observer.kappa";
    const std::string A_KEY = "Obesrver.alpha";
    const std::string B_KEY = "Observer.beta";
    
    // Other string constants
    const std::string MODULE_NAME = "UnscentedKalmanFilter";
    
    // Constructor
    UnscentedKalmanFilter::UnscentedKalmanFilter(Model * model, const Matrix Q, const Matrix R) : Observer() {
        // Set the model (use the function so that it sets up the other stuff)
        setModel(model);
        
        // Set Q, R
        m_Q = Q;
        m_R = R;
        
        // Check that Q and R are the right size
        if (m_Q.rows() != m_Q.cols() || m_Q.rows() != pModel->getNumStates()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Q does not have the right number of values");
            throw std::range_error("Q does not have the right number of values");
        }
        if (m_R.rows() != m_R.cols() || m_R.rows() != pModel->getNumOutputs()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "R does not have the right number of values");
            throw std::range_error("R does not have the right number of values");
        }
    }
    
    // Set model
    void UnscentedKalmanFilter::setModel(Model * model) {
        // Set the model pointer
        pModel = model;
        
        // Set up variables that are dependent on the model
        m_xEstimated.resize(pModel->getNumStates());
        m_uOld.resize(pModel->getNumInputs());
        m_zEstimated.resize(pModel->getNumOutputs());
        
        // Set up sigma point matrices and weights for x
        m_sigmaX.M.resize(pModel->getNumStates(), 2 * pModel->getNumStates() + 1);
        m_sigmaX.w.resize(2 * pModel->getNumStates() + 1);
        
        // Set kappa to default value, unless has been configured already
        if (std::isnan(m_sigmaX.kappa)) {
            m_sigmaX.kappa = 3.0 - pModel->getNumStates();
        }
        // Set alpha to default value, unless has been configured already
        if (std::isnan(m_sigmaX.alpha)) {
            m_sigmaX.alpha = 1;
        }
        // Set beta to default value, unless has been configured already
        if (std::isnan(m_sigmaX.beta)) {
            m_sigmaX.beta = 0;
        }
    }
    
    // GSAPConfigMap-based Constructor
    UnscentedKalmanFilter::UnscentedKalmanFilter(GSAPConfigMap & configMap) : Observer() {
        // Check for required parameters: Q, R
        configMap.checkRequiredParams({ Q_KEY, R_KEY });
        
        // Set Q
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Setting Q");
        std::vector<std::string> QValues = configMap.at(Q_KEY);
        // Make sure it has enough values to fill a square matrix
        if (std::abs(sqrt(QValues.size()) - floor(sqrt(QValues.size()))) > 1e-12) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Q is not a square matrix!");
            throw std::domain_error("Q is not a square matrix!");
        }
        std::size_t dimension = static_cast<std::size_t>(sqrt(QValues.size()));
        m_Q.resize(dimension, dimension);
        unsigned int qValueIndex = 0;
        for (unsigned int row = 0; row < dimension; row++) {
            for (unsigned int col = 0; col < dimension; col++) {
                m_Q[row][col] = std::stod(QValues[qValueIndex]);
                qValueIndex++;
            }
        }
        
        // Set R
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Setting R");
        std::vector<std::string> RValues = configMap.at(R_KEY);
        // Make sure it has enough values to fill a square matrix
        if (std::abs(sqrt(RValues.size()) - floor(sqrt(RValues.size()))) > 1e-12) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "R is not a square matrix!");
            throw std::domain_error("R is not a square matrix!");
        }
        dimension = static_cast<std::size_t>(sqrt(RValues.size()));
        m_R.resize(dimension, dimension);
        unsigned int rValueIndex = 0;
        for (unsigned int row = 0; row < dimension; row++) {
            for (unsigned int col = 0; col < dimension; col++) {
                m_R[row][col] = std::stod(RValues[rValueIndex]);
                rValueIndex++;
            }
        }
        
        // Set kappa (optional)
        if (configMap.includes(K_KEY)) {
            setKappa(std::stod(configMap.at(K_KEY)[0]));
        }
        // Set alpha (optional)
        if (configMap.includes(K_KEY)) {
            setAlpha(std::stod(configMap.at(A_KEY)[0]));
        }
        // Set beta (optional)
        if (configMap.includes(K_KEY)) {
            setBeta(std::stod(configMap.at(B_KEY)[0]));
        }
        
        log.WriteLine(LOG_INFO, MODULE_NAME, "Created UKF");
    }
    
    // Set kappa value
    void UnscentedKalmanFilter::setKappa(double kappa) {
        m_sigmaX.kappa = kappa;
    }
    
    // Set alpha value
    void UnscentedKalmanFilter::setAlpha(double alpha) {
        m_sigmaX.alpha = alpha;
    }
    
    // Set beta value
    void UnscentedKalmanFilter::setBeta(double beta) {
        m_sigmaX.beta = beta;
    }
    
    // Initialize function (required by Observer interface)
    void UnscentedKalmanFilter::initialize(const double t0, const std::vector<double> & x0,
                                           const std::vector<double> & u0) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initializing");
        
        // Check that model has been set
        if (pModel == NULL) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "UKF does not have a model!");
            throw ConfigurationError("UKF does not have a model!");
        }
        
        // Check that Q and R were set consistent with the model
        if (m_Q.rows() != pModel->getNumStates()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Q does not have the right number of values");
            throw std::range_error("Q does not have the right number of values");
        }
        if (m_R.rows() != pModel->getNumOutputs()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "R does not have the right number of values");
            throw std::range_error("R does not have the right number of values");
        }
        
        // Initialize time, state, inputs
        m_t = t0;
        m_xEstimated = x0;
        m_uOld = u0;
        
        // Initialize P
        m_P = m_Q;
        
        // Compute sigma points for current state estimate
        computeSigmaPoints(m_xEstimated, m_P, m_sigmaX.kappa, m_sigmaX.alpha, m_sigmaX.M, m_sigmaX.w);
        
        // Compute corresponding output estimate
        std::vector<double> zeroNoiseZ(pModel->getNumOutputs());
        pModel->outputEqn(m_t, m_xEstimated, m_uOld, zeroNoiseZ, m_zEstimated);
        
        // Set initialized flag
        m_initialized = true;
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Initialize completed");
    }
    
    // Get state mean
    const std::vector<double> & UnscentedKalmanFilter::getStateMean() const {
        return m_xEstimated;
    }
    
    // Get state covariance
    const Matrix & UnscentedKalmanFilter::getStateCovariance() const {
        return m_P;
    }
    
    // Get output mean
    const std::vector<double> & UnscentedKalmanFilter::getOutputMean() const {
        return m_zEstimated;
    }
    
    // Step function (required by Observer interface)
    void UnscentedKalmanFilter::step(const double newT_s, const std::vector<double> & u,
                                     const std::vector<double> & z) {
        log.WriteLine(LOG_DEBUG, MODULE_NAME, "Starting step");
        
        if (!isInitialized()) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "Called step before initialized");
            throw std::domain_error("UnscentedKalmanFilter::step not initialized");
        }
        
        // Update time
        double dt_s = newT_s - m_t; // Seconds
        m_t = newT_s;
        if (dt_s <= 0) {
            log.WriteLine(LOG_ERROR, MODULE_NAME, "dt is less than or equal to zero");
            throw std::domain_error("UnscentedKalmanFilter::step dt is 0");
        }
        
        unsigned int numStates = pModel->getNumStates();
        unsigned int numOutputs = pModel->getNumOutputs();
        unsigned int numSigmaPoints = static_cast<unsigned int>(m_sigmaX.M.cols());
        
        std::vector<double> zeroNoise(numStates);
        
        // 1. Predict
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - predict");
        
        // Compute sigma points for current state estimate
        computeSigmaPoints(m_xEstimated, m_Q, m_sigmaX.kappa, m_sigmaX.alpha, m_sigmaX.M, m_sigmaX.w);
        
        // Propagate sigma points through state equation
        Matrix Xkk1(numStates, numSigmaPoints);
        for (unsigned int i = 0; i < numSigmaPoints; i++) {
            // Get ith sigma point
            std::vector<double> x = static_cast<std::vector<double>>(m_sigmaX.M.col(i));
            // Apply state equation
            pModel->stateEqn(newT_s, x, m_uOld, zeroNoise, dt_s);
            // Set column in Xkk1
            Xkk1.col(i, x);
        }
        
        // Recombine weighted sigma points to produce predicted state and covariance
        std::vector<double> xkk1 = static_cast<std::vector<double>>(Xkk1.weightedMean(Matrix(m_sigmaX.w)));
        Matrix Pkk1 = Xkk1.weightedCovariance(Matrix(m_sigmaX.w), m_sigmaX.alpha, m_sigmaX.beta) + m_Q;
        
        // Propagate sigma points through output equation
        Matrix Zkk1(numOutputs, numSigmaPoints);
        for (unsigned int i = 0; i < numSigmaPoints; i++) {
            // Get ith predicted sigma point
            std::vector<double> zkk1(numOutputs);
            // Apply state equation
            pModel->outputEqn(newT_s, static_cast<std::vector<double>>(Xkk1.col(i)), u, zeroNoise, zkk1);
            // Set column in Zkk1
            Zkk1.col(i, zkk1);
        }
        
        // Recombine weighted sigma points to produce predicted measurement and covariance
        std::vector<double> zkk1 = static_cast<std::vector<double>>(Zkk1.weightedMean(Matrix(m_sigmaX.w)));
        Matrix Pzz = Zkk1.weightedCovariance(Matrix(m_sigmaX.w), m_sigmaX.alpha, m_sigmaX.beta) + m_R;
        
        // 2. Update
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Starting step - update");
        
        // Compute state-output cross-covariance matrix
        Matrix Pxz(numStates, numOutputs);
        for (unsigned int i = 0; i < numSigmaPoints; i++) {
            Matrix columnx(numStates, 1);
            Matrix columnz(numOutputs, 1);
            columnx.col(0, Xkk1.col(i));
            columnz.col(0, Zkk1.col(i));
            
            Matrix xkk1m(numStates, 1);
            Matrix zkk1m(numOutputs, 1);
            xkk1m.col(0, xkk1);
            zkk1m.col(0, zkk1);
            
            Matrix diffx = columnx - xkk1m;
            Matrix diffzT = (columnz - zkk1m).transpose();
            Matrix temp = diffx*diffzT;
            Pxz = Pxz + temp*m_sigmaX.w[i];
        }
        
        
        // Compute Kalman gain
        Matrix Kk = Pxz*Pzz.inverse();
        
        // Compute state estimate
        Matrix xkk1m(numStates, 1);
        Matrix zkk1m(numOutputs, 1);
        Matrix zm(numOutputs, 1);
        xkk1m.col(0, xkk1);
        zkk1m.col(0, zkk1);
        zm.col(0, z);
        Matrix xk1m = xkk1m + Kk*(zm - zkk1m);
        m_xEstimated = static_cast<std::vector<double>>(xk1m.col(0));
        
        // Compute output estimate
        std::vector<double> zeroNoiseZ(numOutputs);
        pModel->outputEqn(newT_s, m_xEstimated, u, zeroNoiseZ, m_zEstimated);
        
        // Compute covariance
        m_P = Pkk1 - Kk*Pzz*Kk.transpose();
        
        // Update uOld
        m_uOld = u;
    }
    
    // Compute sigma points: implements symmetric unscented transform, with scaling
    // Outputs are X and w, which are passed by reference
    void UnscentedKalmanFilter::computeSigmaPoints(const std::vector<double> & mx,
                                                   const Matrix & Pxx, const double kappa,
                                                   const double alpha,
                                                   Matrix & X, std::vector<double> & w) {
        log.WriteLine(LOG_TRACE, MODULE_NAME, "Computing sigma points");
        
        // Assumes that sigma points have been set up correctly within the constructor
        unsigned int numStates = static_cast<unsigned int>(mx.size());
        unsigned int numSigmaPoints = static_cast<unsigned int>(X.cols());
        
        // First sigma point is the mean
        for (unsigned int i = 0; i < numStates; i++) {
            X[i][0] = mx[i];
        }
        
        // Compute a matrix square root using Cholesky decomposition
        Matrix nkPxx = Pxx;
        for (unsigned int i = 0; i < nkPxx.rows(); i++) {
            for (unsigned int j = 0; j < nkPxx.cols(); j++) {
                nkPxx[i][j] *= (numStates + kappa);
            }
        }
        Matrix matrixSq = nkPxx.chol();
        
        // For sigma points 2 to n+1, it is mx + ith column of matrix square root
        for (unsigned int i = 0; i < numStates; i++) {
            // Set column
            for (unsigned int j = 0; j < numStates; j++) {
                X[i][j + 1] = mx[i] + matrixSq[i][j];
            }
        }
        
        // For sigma points n+2 to 2n+1, it is mx - ith column of matrix square root
        for (unsigned int i = 0; i < numStates; i++) {
            // Set column
            for (unsigned int j = 0; j < numStates; j++) {
                X[i][j + numStates + 1] = mx[i] - matrixSq[i][j];
            }
        }
        
        // w(1) is kappa/(n+kappa)
        w[0] = kappa / (numStates + kappa);
        
        // Rest of w are 0.5/(n+kappa)
        for (unsigned int i = 1; i < numSigmaPoints; i++) {
            w[i] = 0.5 / (numStates + kappa);
        }
        
        // Scale the sigma points
        // 1. Xi' = X0 + alpha*(Xi-X0)
        Matrix X0 = X.col(0);
        for (unsigned int i = 1; i < numSigmaPoints; i++) {
            X.col(i,X0 + alpha*(X.col(i)-X0));
        }
        
        // 2. W0' = W0/alpha^2 + (1/alpha^2-1)
        //    Wi' = Wi/alpha^2
        w[0] = w[0]/alpha/alpha + (1/alpha/alpha-1);
        for (unsigned int i = 1; i < numSigmaPoints; i++) {
            w[i] = w[i]/alpha/alpha;
        }
    }
    
    std::vector<UData> UnscentedKalmanFilter::getStateEstimate() const {
        std::vector<UData> state(pModel->getNumStates());
        for (unsigned int i = 0; i < pModel->getNumStates(); i++) {
            state[i].uncertainty(UType::MeanCovar);
            state[i].npoints(pModel->getNumStates());
            state[i][MEAN] = m_xEstimated[i];
            state[i][COVAR()] = static_cast<std::vector<double>>(m_P.row(i));
        }
        return state;
    }
    
    // Print status
    void UnscentedKalmanFilter::print() const {
        std::cout << "xEsimated: " << std::endl;
        for (size_t i = 0; i < m_xEstimated.size(); i++) {
            std::cout << m_xEstimated[i] << " ";
        } std::cout << std::endl;
        std::cout << "zEstimated: " << std::endl;
        for (size_t i = 0; i < m_zEstimated.size(); i++) {
            std::cout << m_zEstimated[i] << " ";
        } std::cout << std::endl;
        std::cout << "P: " << std::endl;
        std::cout << m_P;
    }
}
