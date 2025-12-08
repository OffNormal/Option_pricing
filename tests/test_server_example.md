# HTTP Server Testing Guide

## Running the Server

To start the OptionPricer HTTP server:

```bash
# From the build directory
cd build6/Release
./option_pricer.exe --port 8080
```

The server will start and display:
```
OptionPricer - Option Pricing Platform
Version: 1.0.0

Starting OptionPricer HTTP server on port 8080...
API endpoints:
  POST http://localhost:8080/api/price
  POST http://localhost:8080/api/visualize
```

## Testing with curl

### 1. Test Root Endpoint (GET /)

```bash
curl http://localhost:8080/
```

Expected response:
```json
{
  "service": "OptionPricer API",
  "version": "1.0",
  "endpoints": [
    {
      "path": "/api/price",
      "method": "POST",
      "description": "Calculate option price and Greeks"
    },
    {
      "path": "/api/visualize",
      "method": "POST",
      "description": "Generate visualization data for option pricing"
    }
  ]
}
```

### 2. Test Price Calculation (POST /api/price)

#### Black-Scholes Model:

```bash
curl -X POST http://localhost:8080/api/price \
  -H "Content-Type: application/json" \
  -d "{
    \"model\": \"black-scholes\",
    \"spot_price\": 100.0,
    \"strike_price\": 105.0,
    \"time_to_maturity\": 1.0,
    \"risk_free_rate\": 0.05,
    \"volatility\": 0.2,
    \"option_type\": \"call\"
  }"
```

Expected response:
```json
{
  "success": true,
  "model": "Black-Scholes",
  "option_price": 8.9162,
  "greeks": {
    "delta": 0.5596,
    "gamma": 0.0184,
    "theta": -6.4135,
    "vega": 36.7311,
    "rho": 51.8644
  },
  "computation_time_ms": 1.234
}
```

#### Binomial Tree Model:

```bash
curl -X POST http://localhost:8080/api/price \
  -H "Content-Type: application/json" \
  -d "{
    \"model\": \"binomial-tree\",
    \"binomial_steps\": 100,
    \"spot_price\": 100.0,
    \"strike_price\": 105.0,
    \"time_to_maturity\": 1.0,
    \"risk_free_rate\": 0.05,
    \"volatility\": 0.2,
    \"option_type\": \"call\"
  }"
```

### 3. Test Visualization Data (POST /api/visualize)

```bash
curl -X POST http://localhost:8080/api/visualize \
  -H "Content-Type: application/json" \
  -d "{
    \"model\": \"black-scholes\",
    \"spot_price\": 100.0,
    \"strike_price\": 105.0,
    \"time_to_maturity\": 1.0,
    \"risk_free_rate\": 0.05,
    \"volatility\": 0.2,
    \"option_type\": \"call\",
    \"spot_min\": 50.0,
    \"spot_max\": 150.0,
    \"points\": 50
  }"
```

Expected response includes price curve and Greeks curves data.

### 4. Test Error Handling

#### Invalid Input:

```bash
curl -X POST http://localhost:8080/api/price \
  -H "Content-Type: application/json" \
  -d "{
    \"model\": \"black-scholes\",
    \"spot_price\": -100.0,
    \"strike_price\": 105.0,
    \"time_to_maturity\": 1.0,
    \"risk_free_rate\": 0.05,
    \"volatility\": 0.2,
    \"option_type\": \"call\"
  }"
```

Expected response:
```json
{
  "success": false,
  "error": "Invalid input parameters",
  "details": [
    "Spot price must be positive"
  ]
}
```

#### Invalid Model:

```bash
curl -X POST http://localhost:8080/api/price \
  -H "Content-Type: application/json" \
  -d "{
    \"model\": \"invalid-model\",
    \"spot_price\": 100.0,
    \"strike_price\": 105.0,
    \"time_to_maturity\": 1.0,
    \"risk_free_rate\": 0.05,
    \"volatility\": 0.2,
    \"option_type\": \"call\"
  }"
```

Expected response:
```json
{
  "success": false,
  "error": "Invalid model type",
  "details": [
    "Supported models: black-scholes, binomial-tree"
  ]
}
```

## CORS Support

The server includes CORS headers in all responses:
- `Access-Control-Allow-Origin: *`
- `Access-Control-Allow-Methods: GET, POST, OPTIONS`
- `Access-Control-Allow-Headers: Content-Type`

This allows the server to be accessed from web browsers running on different domains.

## Stopping the Server

Press `Ctrl+C` to gracefully shut down the server.
