// OptionPricer Frontend JavaScript

// DOM元素
const optionForm = document.getElementById('optionForm');
const modelSelect = document.getElementById('model');
const binomialStepsGroup = document.getElementById('binomialStepsGroup');
const errorDisplay = document.getElementById('errorDisplay');
const errorList = document.getElementById('errorList');
const calculateBtn = document.getElementById('calculateBtn');
const resetBtn = document.getElementById('resetBtn');

// 初始化
document.addEventListener('DOMContentLoaded', () => {
    setupEventListeners();
    loadDefaultValues();
});

/**
 * 设置事件监听器
 */
function setupEventListeners() {
    // 模型选择变化时显示/隐藏二叉树步数输入
    modelSelect.addEventListener('change', handleModelChange);
    
    // 表单提交
    optionForm.addEventListener('submit', handleFormSubmit);
    
    // 重置按钮
    resetBtn.addEventListener('click', handleFormReset);
    
    // 实时输入验证
    const inputs = optionForm.querySelectorAll('input[type="number"]');
    inputs.forEach(input => {
        input.addEventListener('blur', () => validateInput(input));
        input.addEventListener('input', () => clearInputError(input));
    });
}

/**
 * 加载默认值
 */
function loadDefaultValues() {
    // 设置一些合理的默认值
    document.getElementById('spot_price').value = '100';
    document.getElementById('strike_price').value = '105';
    document.getElementById('time_to_maturity').value = '1.0';
    document.getElementById('risk_free_rate').value = '0.05';
    document.getElementById('volatility').value = '0.20';
}

/**
 * 处理模型选择变化
 */
function handleModelChange() {
    const selectedModel = modelSelect.value;
    
    if (selectedModel === 'binomial-tree') {
        binomialStepsGroup.style.display = 'block';
        document.getElementById('binomial_steps').required = true;
    } else {
        binomialStepsGroup.style.display = 'none';
        document.getElementById('binomial_steps').required = false;
    }
}

// ============================================================================
// API通信功能
// ============================================================================

/**
 * 调用价格计算API
 * @param {Object} formData 表单数据
 * @returns {Promise<Object>} API响应数据
 */
async function callPriceAPI(formData) {
    const apiUrl = '/api/price';
    
    // 构建请求体
    const requestBody = {
        model: formData.model,
        option_type: formData.option_type,
        spot_price: formData.spot_price,
        strike_price: formData.strike_price,
        time_to_maturity: formData.time_to_maturity,
        risk_free_rate: formData.risk_free_rate,
        volatility: formData.volatility
    };
    
    // 如果是二叉树模型，添加步数参数
    if (formData.model === 'binomial-tree' && formData.binomial_steps) {
        requestBody.binomial_steps = formData.binomial_steps;
    }
    
    try {
        // 发送POST请求，设置5秒超时
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 5000);
        
        const response = await fetch(apiUrl, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestBody),
            signal: controller.signal
        });
        
        clearTimeout(timeoutId);
        
        // 解析JSON响应
        const data = await response.json();
        
        // Проверка HTTP статус-кода
        if (!response.ok) {
            // Сервер вернул ошибку
            return {
                success: false,
                error: data.error || 'Ошибка сервера',
                details: data.details || []
            };
        }
        
        return data;
        
    } catch (error) {
        // Обработка сетевых ошибок
        if (error.name === 'AbortError') {
            throw new Error('Тайм-аут запроса: расчет занял более 5 секунд, попробуйте уменьшить количество шагов');
        } else if (error instanceof TypeError && error.message.includes('fetch')) {
            throw new Error('Сетевая ошибка: не удается подключиться к серверу, убедитесь, что сервер запущен');
        } else {
            throw new Error('Ошибка запроса: ' + error.message);
        }
    }
}

/**
 * 调用可视化数据生成API
 * @param {Object} formData 表单数据
 * @returns {Promise<Object>} API响应数据
 */
async function callVisualizationAPI(formData) {
    const apiUrl = '/api/visualize';
    
    // 构建请求体
    const requestBody = {
        model: formData.model,
        option_type: formData.option_type,
        spot_price: formData.spot_price,
        strike_price: formData.strike_price,
        time_to_maturity: formData.time_to_maturity,
        risk_free_rate: formData.risk_free_rate,
        volatility: formData.volatility,
        points: 50  // 默认生成50个数据点
    };
    
    // 如果是二叉树模型，添加步数参数
    if (formData.model === 'binomial-tree' && formData.binomial_steps) {
        requestBody.binomial_steps = formData.binomial_steps;
    }
    
    try {
        // 发送POST请求，设置10秒超时（可视化计算可能需要更长时间）
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 10000);
        
        const response = await fetch(apiUrl, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestBody),
            signal: controller.signal
        });
        
        clearTimeout(timeoutId);
        
        // 解析JSON响应
        const data = await response.json();
        
        // Проверка HTTP статус-кода
        if (!response.ok) {
            // Сервер вернул ошибку
            return {
                success: false,
                error: data.error || 'Не удалось создать данные визуализации',
                details: data.details || []
            };
        }
        
        return data;
        
    } catch (error) {
        // Обработка сетевых ошибок
        if (error.name === 'AbortError') {
            throw new Error('Тайм-аут визуализации: расчет занял более 10 секунд');
        } else if (error instanceof TypeError && error.message.includes('fetch')) {
            throw new Error('Сетевая ошибка: не удается подключиться к серверу');
        } else {
            throw new Error('Ошибка запроса визуализации: ' + error.message);
        }
    }
}

/**
 * 处理表单提交
 */
async function handleFormSubmit(event) {
    event.preventDefault();
    
    // 清除之前的错误
    hideErrors();
    
    // 验证表单
    const validationErrors = validateForm();
    if (validationErrors.length > 0) {
        showErrors(validationErrors);
        return;
    }
    
    // 收集表单数据
    const formData = collectFormData();
    
    // 显示加载状态
    setLoadingState(true);
    
    try {
        // 调用价格计算API
        const priceResult = await callPriceAPI(formData);
        
        // 检查是否成功
        if (!priceResult.success) {
            const errorMessages = priceResult.details || [priceResult.error || '计算失败'];
            showErrors(errorMessages);
            return;
        }
        
        // 显示结果
        displayResults(priceResult);
        
        // Вызов API визуализации
        try {
            const visualizationResult = await callVisualizationAPI(formData);
            
            if (visualizationResult.success && visualizationResult.visualization_data) {
                displayVisualization(visualizationResult.visualization_data);
            }
        } catch (vizError) {
            console.warn('Не удалось создать данные визуализации:', vizError);
            // Ошибка визуализации не влияет на отображение основных результатов
        }
        
    } catch (error) {
        showErrors([error.message || 'Произошла неизвестная ошибка']);
    } finally {
        setLoadingState(false);
    }
}

/**
 * 处理表单重置
 */
function handleFormReset() {
    optionForm.reset();
    hideErrors();
    hideResults();
    loadDefaultValues();
    handleModelChange(); // 重置模型相关显示
}

/**
 * 验证整个表单
 * @returns {Array} 错误消息数组
 */
function validateForm() {
    const errors = [];
    
    // 获取表单值
    const spotPrice = parseFloat(document.getElementById('spot_price').value);
    const strikePrice = parseFloat(document.getElementById('strike_price').value);
    const timeToMaturity = parseFloat(document.getElementById('time_to_maturity').value);
    const riskFreeRate = parseFloat(document.getElementById('risk_free_rate').value);
    const volatility = parseFloat(document.getElementById('volatility').value);
    const model = modelSelect.value;
    const binomialSteps = parseInt(document.getElementById('binomial_steps').value);
    
    // Проверка цены базового актива
    if (isNaN(spotPrice) || spotPrice <= 0) {
        errors.push('Цена базового актива должна быть положительной');
    }
    
    // Проверка цены исполнения
    if (isNaN(strikePrice) || strikePrice <= 0) {
        errors.push('Цена исполнения должна быть положительной');
    }
    
    // Проверка времени до истечения
    if (isNaN(timeToMaturity) || timeToMaturity <= 0) {
        errors.push('Время до истечения должно быть положительным');
    }
    
    // Проверка безрисковой ставки
    if (isNaN(riskFreeRate)) {
        errors.push('Безрисковая ставка должна быть действительным числом');
    }
    
    // Проверка волатильности
    if (isNaN(volatility) || volatility < 0) {
        errors.push('Волатильность должна быть неотрицательной');
    }
    
    // Проверка количества шагов (если выбрана биномиальная модель)
    if (model === 'binomial-tree') {
        if (isNaN(binomialSteps) || binomialSteps < 10 || binomialSteps > 1000) {
            errors.push('Количество шагов должно быть от 10 до 1000');
        }
    }
    
    // Дополнительная бизнес-логика проверки
    if (!isNaN(volatility) && volatility > 2.0) {
        errors.push('Предупреждение: волатильность превышает 200%, результаты могут быть неточными');
    }
    
    if (!isNaN(timeToMaturity) && timeToMaturity > 10) {
        errors.push('Предупреждение: время до истечения превышает 10 лет, результаты могут быть неточными');
    }
    
    return errors;
}

/**
 * 验证单个输入字段
 * @param {HTMLInputElement} input 输入元素
 */
function validateInput(input) {
    const value = parseFloat(input.value);
    const name = input.name;
    
    let isValid = true;
    let errorMessage = '';
    
    switch (name) {
        case 'spot_price':
        case 'strike_price':
            if (isNaN(value) || value <= 0) {
                isValid = false;
                errorMessage = 'Должно быть положительным числом';
            }
            break;
            
        case 'time_to_maturity':
            if (isNaN(value) || value <= 0) {
                isValid = false;
                errorMessage = 'Должно быть положительным числом';
            }
            break;
            
        case 'volatility':
            if (isNaN(value) || value < 0) {
                isValid = false;
                errorMessage = 'Должно быть неотрицательным числом';
            }
            break;
            
        case 'risk_free_rate':
            if (isNaN(value)) {
                isValid = false;
                errorMessage = 'Должно быть действительным числом';
            }
            break;
            
        case 'binomial_steps':
            if (isNaN(value) || value < 10 || value > 1000) {
                isValid = false;
                errorMessage = 'Должно быть от 10 до 1000';
            }
            break;
    }
    
    if (!isValid) {
        input.classList.add('invalid');
        showInputError(input, errorMessage);
    } else {
        input.classList.remove('invalid');
        clearInputError(input);
    }
    
    return isValid;
}

/**
 * 显示输入字段错误
 * @param {HTMLInputElement} input 输入元素
 * @param {string} message 错误消息
 */
function showInputError(input, message) {
    // 移除已存在的错误消息
    clearInputError(input);
    
    // 创建错误消息元素
    const errorSpan = document.createElement('span');
    errorSpan.className = 'input-error';
    errorSpan.style.color = '#e74c3c';
    errorSpan.style.fontSize = '0.85rem';
    errorSpan.style.marginTop = '5px';
    errorSpan.style.display = 'block';
    errorSpan.textContent = message;
    
    // 插入错误消息
    input.parentElement.appendChild(errorSpan);
}

/**
 * 清除输入字段错误
 * @param {HTMLInputElement} input 输入元素
 */
function clearInputError(input) {
    const errorSpan = input.parentElement.querySelector('.input-error');
    if (errorSpan) {
        errorSpan.remove();
    }
}

/**
 * 收集表单数据
 * @returns {Object} 表单数据对象
 */
function collectFormData() {
    const formData = {
        model: modelSelect.value,
        option_type: document.getElementById('option_type').value,
        spot_price: parseFloat(document.getElementById('spot_price').value),
        strike_price: parseFloat(document.getElementById('strike_price').value),
        time_to_maturity: parseFloat(document.getElementById('time_to_maturity').value),
        risk_free_rate: parseFloat(document.getElementById('risk_free_rate').value),
        volatility: parseFloat(document.getElementById('volatility').value)
    };
    
    // 如果是二叉树模型，添加步数参数
    if (formData.model === 'binomial-tree') {
        formData.binomial_steps = parseInt(document.getElementById('binomial_steps').value);
    }
    
    return formData;
}

/**
 * 显示错误消息
 * @param {Array} errors 错误消息数组
 */
function showErrors(errors) {
    errorList.innerHTML = '';
    
    errors.forEach(error => {
        const li = document.createElement('li');
        li.textContent = error;
        errorList.appendChild(li);
    });
    
    errorDisplay.style.display = 'block';
    
    // 滚动到错误显示区域
    errorDisplay.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
}

/**
 * 隐藏错误消息
 */
function hideErrors() {
    errorDisplay.style.display = 'none';
    errorList.innerHTML = '';
}

/**
 * 设置加载状态
 * @param {boolean} isLoading 是否正在加载
 */
function setLoadingState(isLoading) {
    calculateBtn.disabled = isLoading;
    resetBtn.disabled = isLoading;
    
    // 禁用所有输入字段
    const inputs = optionForm.querySelectorAll('input, select');
    inputs.forEach(input => {
        input.disabled = isLoading;
    });
    
    if (isLoading) {
        calculateBtn.innerHTML = '<span class="loading"></span> Расчет...';
        calculateBtn.style.cursor = 'wait';
    } else {
        calculateBtn.innerHTML = 'Рассчитать цену опциона';
        calculateBtn.style.cursor = 'pointer';
    }
}

/**
 * 格式化数字为四位小数
 * @param {number} value 数值
 * @returns {string} 格式化后的字符串
 */
function formatNumber(value) {
    if (typeof value !== 'number' || isNaN(value)) {
        return 'N/A';
    }
    return value.toFixed(4);
}

/**
 * 生成模拟可视化数据（用于测试，实际数据将从API获取）
 * @param {Object} formData 表单数据
 * @returns {Object} 可视化数据对象
 */
function generateMockVisualizationData(formData) {
    const spotPrice = formData.spot_price;
    const strikePrice = formData.strike_price;
    const volatility = formData.volatility;
    const timeToMaturity = formData.time_to_maturity;
    const riskFreeRate = formData.risk_free_rate;
    const isCall = formData.option_type === 'call';
    
    // 生成标的资产价格范围（围绕当前价格的±40%）
    const spotMin = spotPrice * 0.6;
    const spotMax = spotPrice * 1.4;
    const numPoints = 50;
    const spotStep = (spotMax - spotMin) / (numPoints - 1);
    
    const priceX = [];
    const priceY = [];
    const deltaY = [];
    const gammaY = [];
    const thetaY = [];
    const vegaY = [];
    const rhoY = [];
    
    // 简化的Black-Scholes计算（用于生成模拟数据）
    for (let i = 0; i < numPoints; i++) {
        const S = spotMin + i * spotStep;
        priceX.push(S);
        
        // 简化的期权价格计算
        const moneyness = S / strikePrice;
        let optionPrice;
        
        if (isCall) {
            optionPrice = Math.max(0, S - strikePrice * Math.exp(-riskFreeRate * timeToMaturity));
            optionPrice += volatility * Math.sqrt(timeToMaturity) * S * 0.4 * Math.exp(-Math.pow(Math.log(moneyness), 2) / 2);
        } else {
            optionPrice = Math.max(0, strikePrice * Math.exp(-riskFreeRate * timeToMaturity) - S);
            optionPrice += volatility * Math.sqrt(timeToMaturity) * S * 0.4 * Math.exp(-Math.pow(Math.log(moneyness), 2) / 2);
        }
        
        priceY.push(Math.max(0, optionPrice));
        
        // 简化的希腊值计算
        const d1 = (Math.log(S / strikePrice) + (riskFreeRate + 0.5 * volatility * volatility) * timeToMaturity) / 
                   (volatility * Math.sqrt(timeToMaturity));
        const normCdf = 0.5 * (1 + Math.tanh(d1 / Math.sqrt(2)));
        const normPdf = Math.exp(-0.5 * d1 * d1) / Math.sqrt(2 * Math.PI);
        
        // Delta
        const delta = isCall ? normCdf : normCdf - 1;
        deltaY.push(delta);
        
        // Gamma
        const gamma = normPdf / (S * volatility * Math.sqrt(timeToMaturity));
        gammaY.push(gamma);
        
        // Theta (简化)
        const theta = -(S * normPdf * volatility) / (2 * Math.sqrt(timeToMaturity)) - 
                      riskFreeRate * strikePrice * Math.exp(-riskFreeRate * timeToMaturity) * (isCall ? normCdf : (normCdf - 1));
        thetaY.push(theta / 365); // 转换为每日
        
        // Vega
        const vega = S * normPdf * Math.sqrt(timeToMaturity);
        vegaY.push(vega / 100); // 转换为1%波动率变化
        
        // Rho
        const rho = strikePrice * timeToMaturity * Math.exp(-riskFreeRate * timeToMaturity) * (isCall ? normCdf : (normCdf - 1));
        rhoY.push(rho / 100); // 转换为1%利率变化
    }
    
    return {
        price_curve: {
            x: priceX,
            y: priceY
        },
        greeks_curves: {
            delta: { x: priceX, y: deltaY },
            gamma: { x: priceX, y: gammaY },
            theta: { x: priceX, y: thetaY },
            vega: { x: priceX, y: vegaY },
            rho: { x: priceX, y: rhoY }
        }
    };
}

/**
 * 显示计算结果
 * @param {Object} result 计算结果对象
 */
function displayResults(result) {
    const resultsSection = document.getElementById('resultsSection');
    const resultsContent = document.getElementById('resultsContent');
    
    // Построение HTML результатов
    const html = `
        <div class="results-header">
            <div class="model-info">
                Модель: ${result.model || 'Unknown'}
            </div>
            <div class="computation-time">
                Время расчета: ${result.computation_time_ms || 0} мс
            </div>
        </div>
        
        <div class="results-grid">
            <!-- Карточка цены опциона -->
            <div class="result-card">
                <h3>Цена опциона</h3>
                <div class="price-highlight">
                    <span class="price-label">Стоимость опциона</span>
                    ${formatNumber(result.option_price)}
                </div>
                <table class="result-table">
                    <thead>
                        <tr>
                            <th>Параметр</th>
                            <th>Значение</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr>
                            <td>Цена опциона</td>
                            <td class="result-value">${formatNumber(result.option_price)}</td>
                        </tr>
                    </tbody>
                </table>
            </div>
            
            <!-- Карточка греков -->
            ${result.greeks ? `
            <div class="result-card">
                <h3>Греки (Greeks)</h3>
                <table class="result-table">
                    <thead>
                        <tr>
                            <th>Грек</th>
                            <th>Значение</th>
                            <th>Описание</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr>
                            <td>Delta (Δ)</td>
                            <td class="result-value">${formatNumber(result.greeks.delta)}</td>
                            <td>Чувствительность к цене</td>
                        </tr>
                        <tr>
                            <td>Gamma (Γ)</td>
                            <td class="result-value">${formatNumber(result.greeks.gamma)}</td>
                            <td>Скорость изменения Delta</td>
                        </tr>
                        <tr>
                            <td>Theta (Θ)</td>
                            <td class="result-value">${formatNumber(result.greeks.theta)}</td>
                            <td>Временной распад</td>
                        </tr>
                        <tr>
                            <td>Vega (ν)</td>
                            <td class="result-value">${formatNumber(result.greeks.vega)}</td>
                            <td>Чувствительность к волатильности</td>
                        </tr>
                        <tr>
                            <td>Rho (ρ)</td>
                            <td class="result-value">${formatNumber(result.greeks.rho)}</td>
                            <td>Чувствительность к ставке</td>
                        </tr>
                    </tbody>
                </table>
            </div>
            ` : ''}
        </div>
    `;
    
    // 更新内容并显示结果区域
    resultsContent.innerHTML = html;
    resultsSection.style.display = 'block';
    
    // 平滑滚动到结果区域
    resultsSection.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

/**
 * 隐藏结果显示
 */
function hideResults() {
    const resultsSection = document.getElementById('resultsSection');
    resultsSection.style.display = 'none';
    hideVisualization();
}

// ============================================================================
// 可视化图表功能
// ============================================================================

// 存储图表实例
let priceChartInstance = null;
let greeksChartInstance = null;

/**
 * 显示可视化图表
 * @param {Object} visualizationData 可视化数据对象
 */
function displayVisualization(visualizationData) {
    if (!visualizationData) {
        return;
    }
    
    const visualizationSection = document.getElementById('visualizationSection');
    
    // 显示可视化区域
    visualizationSection.style.display = 'block';
    
    // 渲染价格曲线图表
    if (visualizationData.price_curve) {
        renderPriceChart(visualizationData.price_curve);
    }
    
    // 渲染希腊值曲线图表
    if (visualizationData.greeks_curves) {
        renderGreeksChart(visualizationData.greeks_curves);
    }
    
    // 平滑滚动到可视化区域
    visualizationSection.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

/**
 * 隐藏可视化图表
 */
function hideVisualization() {
    const visualizationSection = document.getElementById('visualizationSection');
    visualizationSection.style.display = 'none';
    
    // 销毁现有图表实例
    if (priceChartInstance) {
        priceChartInstance.destroy();
        priceChartInstance = null;
    }
    
    if (greeksChartInstance) {
        greeksChartInstance.destroy();
        greeksChartInstance = null;
    }
}

/**
 * 渲染价格曲线图表
 * @param {Object} priceData 价格曲线数据 {x: [], y: []}
 */
function renderPriceChart(priceData) {
    const ctx = document.getElementById('priceChart');
    
    // 销毁现有图表
    if (priceChartInstance) {
        priceChartInstance.destroy();
    }
    
    // 创建新图表
    priceChartInstance = new Chart(ctx, {
        type: 'line',
        data: {
            labels: priceData.x,
            datasets: [{
                label: 'Цена опциона',
                data: priceData.y,
                borderColor: '#667eea',
                backgroundColor: 'rgba(102, 126, 234, 0.1)',
                borderWidth: 3,
                fill: true,
                tension: 0.4,
                pointRadius: 3,
                pointHoverRadius: 6,
                pointBackgroundColor: '#667eea',
                pointBorderColor: '#fff',
                pointBorderWidth: 2
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: true,
            plugins: {
                legend: {
                    display: true,
                    position: 'top',
                    labels: {
                        font: {
                            size: 14,
                            weight: '600'
                        },
                        color: '#444',
                        padding: 15
                    }
                },
                tooltip: {
                    mode: 'index',
                    intersect: false,
                    backgroundColor: 'rgba(0, 0, 0, 0.8)',
                    titleFont: {
                        size: 14,
                        weight: '600'
                    },
                    bodyFont: {
                        size: 13
                    },
                    padding: 12,
                    cornerRadius: 6,
                    callbacks: {
                        label: function(context) {
                            return 'Цена опциона: ' + context.parsed.y.toFixed(4);
                        }
                    }
                }
            },
            scales: {
                x: {
                    title: {
                        display: true,
                        text: '标的资产价格 (S)',
                        font: {
                            size: 14,
                            weight: '600'
                        },
                        color: '#444',
                        padding: 10
                    },
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)',
                        drawBorder: true,
                        borderColor: '#ccc'
                    },
                    ticks: {
                        font: {
                            size: 12
                        },
                        color: '#666',
                        callback: function(value, index, values) {
                            return this.getLabelForValue(value).toFixed(2);
                        }
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: 'Цена опциона',
                        font: {
                            size: 14,
                            weight: '600'
                        },
                        color: '#444',
                        padding: 10
                    },
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)',
                        drawBorder: true,
                        borderColor: '#ccc'
                    },
                    ticks: {
                        font: {
                            size: 12
                        },
                        color: '#666',
                        callback: function(value) {
                            return value.toFixed(2);
                        }
                    }
                }
            },
            interaction: {
                mode: 'nearest',
                axis: 'x',
                intersect: false
            }
        }
    });
}

/**
 * 渲染希腊值曲线图表（多条曲线）
 * @param {Object} greeksData 希腊值数据 {delta: {x: [], y: []}, gamma: {x: [], y: []}, ...}
 */
function renderGreeksChart(greeksData) {
    const ctx = document.getElementById('greeksChart');
    
    // 销毁现有图表
    if (greeksChartInstance) {
        greeksChartInstance.destroy();
    }
    
    // 定义希腊值的颜色和标签
    const greeksConfig = {
        delta: { label: 'Delta (Δ)', color: '#667eea', borderWidth: 3 },
        gamma: { label: 'Gamma (Γ)', color: '#f093fb', borderWidth: 3 },
        theta: { label: 'Theta (Θ)', color: '#4facfe', borderWidth: 3 },
        vega: { label: 'Vega (ν)', color: '#43e97b', borderWidth: 3 },
        rho: { label: 'Rho (ρ)', color: '#fa709a', borderWidth: 3 }
    };
    
    // 构建数据集
    const datasets = [];
    for (const [greekName, greekData] of Object.entries(greeksData)) {
        if (greeksConfig[greekName] && greekData && greekData.x && greekData.y) {
            const config = greeksConfig[greekName];
            datasets.push({
                label: config.label,
                data: greekData.y,
                borderColor: config.color,
                backgroundColor: config.color + '20', // 添加透明度
                borderWidth: config.borderWidth,
                fill: false,
                tension: 0.4,
                pointRadius: 2,
                pointHoverRadius: 5,
                pointBackgroundColor: config.color,
                pointBorderColor: '#fff',
                pointBorderWidth: 2
            });
        }
    }
    
    // 使用第一个希腊值的x轴数据作为标签
    const xLabels = greeksData.delta ? greeksData.delta.x : 
                    greeksData.gamma ? greeksData.gamma.x :
                    greeksData.theta ? greeksData.theta.x :
                    greeksData.vega ? greeksData.vega.x :
                    greeksData.rho ? greeksData.rho.x : [];
    
    // 创建新图表
    greeksChartInstance = new Chart(ctx, {
        type: 'line',
        data: {
            labels: xLabels,
            datasets: datasets
        },
        options: {
            responsive: true,
            maintainAspectRatio: true,
            plugins: {
                legend: {
                    display: true,
                    position: 'top',
                    labels: {
                        font: {
                            size: 13,
                            weight: '600'
                        },
                        color: '#444',
                        padding: 12,
                        usePointStyle: true,
                        pointStyle: 'circle'
                    }
                },
                tooltip: {
                    mode: 'index',
                    intersect: false,
                    backgroundColor: 'rgba(0, 0, 0, 0.8)',
                    titleFont: {
                        size: 14,
                        weight: '600'
                    },
                    bodyFont: {
                        size: 12
                    },
                    padding: 12,
                    cornerRadius: 6,
                    callbacks: {
                        label: function(context) {
                            return context.dataset.label + ': ' + context.parsed.y.toFixed(4);
                        }
                    }
                }
            },
            scales: {
                x: {
                    title: {
                        display: true,
                        text: 'Цена базового актива (S)',
                        font: {
                            size: 14,
                            weight: '600'
                        },
                        color: '#444',
                        padding: 10
                    },
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)',
                        drawBorder: true,
                        borderColor: '#ccc'
                    },
                    ticks: {
                        font: {
                            size: 11
                        },
                        color: '#666',
                        callback: function(value, index, values) {
                            return this.getLabelForValue(value).toFixed(2);
                        }
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: 'Значения греков',
                        font: {
                            size: 14,
                            weight: '600'
                        },
                        color: '#444',
                        padding: 10
                    },
                    grid: {
                        color: 'rgba(0, 0, 0, 0.05)',
                        drawBorder: true,
                        borderColor: '#ccc'
                    },
                    ticks: {
                        font: {
                            size: 11
                        },
                        color: '#666',
                        callback: function(value) {
                            return value.toFixed(2);
                        }
                    }
                }
            },
            interaction: {
                mode: 'nearest',
                axis: 'x',
                intersect: false
            }
        }
    });
}

// 导出函数供其他模块使用（如果需要）
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        validateForm,
        collectFormData,
        showErrors,
        hideErrors,
        displayResults,
        hideResults,
        formatNumber,
        displayVisualization,
        hideVisualization,
        renderPriceChart,
        renderGreeksChart
    };
}
