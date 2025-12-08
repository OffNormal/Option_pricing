# Web Frontend Resources

This directory contains the web frontend files for the OptionPricer platform.

## Contents

- `index.html` - Main web interface with input form
- `style.css` - Responsive styling with gradient design
- `app.js` - Frontend JavaScript logic with form validation

## Features Implemented

### Task 14: Web Frontend Interface ✓

- **HTML Structure**: Complete page layout with header, form, and result sections
- **Input Form**: All option parameters with proper labels and help text
  - Model selection (Black-Scholes / Binomial Tree)
  - Conditional binomial steps input (only shown for binomial tree model)
  - Option type selection (Call / Put)
  - All pricing parameters (spot price, strike price, time to maturity, risk-free rate, volatility)
- **Form Validation**: 
  - Client-side validation for all inputs
  - Real-time error display
  - Proper input constraints (min/max values, step sizes)
- **Responsive Design**: Mobile-friendly layout with gradient styling
- **Error Display**: Dedicated error message area with detailed feedback

### Task 15: Frontend Results Display ✓

- **Results Section**: Dynamic results display area
- **Results Header**: Shows model name and computation time
- **Price Display**: Highlighted option price with four decimal precision
- **Greeks Table**: Comprehensive table showing all five Greeks values
  - Delta (Δ) - Price change sensitivity
  - Gamma (Γ) - Delta change rate
  - Theta (Θ) - Time decay
  - Vega (ν) - Volatility sensitivity
  - Rho (ρ) - Interest rate sensitivity
- **Number Formatting**: All values formatted to 4 decimal places
- **Responsive Layout**: Card-based design with proper spacing
- **Smooth Scrolling**: Auto-scroll to results after calculation

## Upcoming Features

- Task 16: Chart.js visualization integration
- Task 17: API communication with backend
- Task 18: Static resource serving optimization

## Usage

The web interface is served by the C++ HTTP server. Start the server and navigate to:
```
http://localhost:8080/
```

The form includes default values for quick testing and comprehensive validation to ensure data quality before submission.
