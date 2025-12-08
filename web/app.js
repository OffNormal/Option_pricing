// OptionPricer Frontend JavaScript

// DOM элементы
const optionForm = document.getElementById('optionForm');
const modelSelect = document.getElementById('model');
const binomialStepsGroup = document.getElementById('binomialStepsGroup');
const errorDisplay = document.getElementById('errorDisplay');
const errorList = document.getElementById('errorList');
const calculateBtn = document.getElementById('calculateBtn');
const resetBtn = document.getElementById('resetBtn');

// Инициализация
document.addEventListener('DOMContentLoaded', () => {
    setupEventListeners();
    loadDefaultValues();
});

/**
 * Настройка обработчиков событий
 */
function setupEventListeners() {
    // Показать/скрыть поле количества шагов при изменении модели
    modelSelect.addEventListener('change', handleModelChange);
    
    // Отправка формы
    optionForm.addEventListener('submit', handleFormSubmit);
    
    // Кнопка сброса
    resetBtn.addEventListener('click', handleFormReset);
    
    // Проверка ввода в реальном времени
    const inputs = optionForm.querySelectorAll('input[type="number"]');
    inputs.forEach(input => {
        input.addEventListener('blur', () => validateInput(input));
        input.addEventListener('input', () => clearInputError(input));
    });
}

/**
 * Загрузка значений по умолчанию
 */
function loadDefaultValues() {
    // Установка разумных значений по умолчанию
    document.getElementById('spot_price').value = '100';
    document.getElementById('strike_price').value = '105';
    document.getElementById('time_to_maturity').value = '1.0';
    document.getElementById('risk_free_rate').value = '0.05';
    document.getElementById('volatility').value = '0.20';
}

/**
 * Обработка изменения выбора модели
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
// Функции API-коммуникации
// ============================================================================

/**
 * Вызов API расчета цены
 * @param {Object} formData Данные формы
 * @returns {Promise<Object>} Данные ответа API
 */
async function callPriceAPI(formData) {
    const apiUrl = '/api/price';
    
    // Построение тела запроса
    const requestBody = {
        model: formData.model,
        option_type: formData.option_type,
        spot_price: formData.spot_price,
        strike_price: formData.strike_price,
        time_to_maturity: formData.time_to_maturity,
        risk_free_rate: formData.risk_free_rate,
        volatility: formData.volatility
    };
    
    // Если биномиальная модель, добавить параметр количества шагов
    if (formData.model === 'binomial-tree' && formData.binomial_steps) {
        requestBody.binomial_steps = formData.binomial_steps;
    }
    
    try {
        // Отправка POST-запроса с тайм-аутом 5 секунд
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
        
        // Парсинг JSON-ответа
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
 * Вызов API генерации данных визуализации
 * @param {Object} formData Данные формы
 * @returns {Promise<Object>} Данные ответа API
 */
async function callVisualizationAPI(formData) {
    const apiUrl = '/api/visualize';
    
    // Построение тела запроса
    const requestBody = {
        model: formData.model,
        option_type: formData.option_type,
        spot_price: formData.spot_price,
        strike_price: formData.strike_price,
        time_to_maturity: formData.time_to_maturity,
        risk_free_rate: formData.risk_free_rate,
        volatility: formData.volatility,
        points: 50  // По умолчанию генерируется 50 точек данных
    };
    
    // Если биномиальная модель, добавить параметр количества шагов
    if (formData.model === 'binomial-tree' && formData.binomial_steps) {
        requestBody.binomial_steps = formData.binomial_steps;
    }
    
    try {
        // Отправка POST-запроса с тайм-аутом 10 секунд (визуализация может занять больше времени)
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
        
        // Парсинг JSON-ответа
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
 * Обработка отправки формы
 */
async function handleFormSubmit(event) {
    event.preventDefault();
    
    // Очистка предыдущих ошибок
    hideErrors();
    
    // Проверка формы
    const validationErrors = validateForm();
    if (validationErrors.length > 0) {
        showErrors(validationErrors);
        return;
    }
    
    // Сбор данных формы
    const formData = collectFormData();
    
    // Отображение состояния загрузки
    setLoadingState(true);
    
    try {
        // Вызов API расчета цены
        const priceResult = await callPriceAPI(formData);
        
        // Проверка успешности
        if (!priceResult.success) {
            const errorMessages = priceResult.details || [priceResult.error || 'Ошибка расчета'];
            showErrors(errorMessages);
            return;
        }
        
        // Отображение результатов
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
 * Обработка сброса формы
 */
function handleFormReset() {
    optionForm.reset();
    hideErrors();
    hideResults();
    loadDefaultValues();
    handleModelChange(); // Сброс отображения, связанного с моделью
}

/**
 * Проверка всей формы
 * @returns {Array} Массив сообщений об ошибках
 */
function validateForm() {
    const errors = [];
    
    // Получение значений формы
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
 * Проверка отдельного поля ввода
 * @param {HTMLInputElement} input Элемент ввода
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
 * Отображение ошибки поля ввода
 * @param {HTMLInputElement} input Элемент ввода
 * @param {string} message Сообщение об ошибке
 */
function showInputError(input, message) {
    // Удаление существующего сообщения об ошибке
    clearInputError(input);
    
    // Создание элемента сообщения об ошибке
    const errorSpan = document.createElement('span');
    errorSpan.className = 'input-error';
    errorSpan.style.color = '#e74c3c';
    errorSpan.style.fontSize = '0.85rem';
    errorSpan.style.marginTop = '5px';
    errorSpan.style.display = 'block';
    errorSpan.textContent = message;
    
    // Вставка сообщения об ошибке
    input.parentElement.appendChild(errorSpan);
}

/**
 * Очистка ошибки поля ввода
 * @param {HTMLInputElement} input Элемент ввода
 */
function clearInputError(input) {
    const errorSpan = input.parentElement.querySelector('.input-error');
    if (errorSpan) {
        errorSpan.remove();
    }
}

/**
 * Сбор данных формы
 * @returns {Object} Объект данных формы
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
    
    // Если биномиальная модель, добавить параметр количества шагов
    if (formData.model === 'binomial-tree') {
        formData.binomial_steps = parseInt(document.getElementById('binomial_steps').value);
    }
    
    return formData;
}

/**
 * Отображение сообщений об ошибках
 * @param {Array} errors Массив сообщений об ошибках
 */
function showErrors(errors) {
    errorList.innerHTML = '';
    
    errors.forEach(error => {
        const li = document.createElement('li');
        li.textContent = error;
        errorList.appendChild(li);
    });
    
    errorDisplay.style.display = 'block';
    
    // Прокрутка к области отображения ошибок
    errorDisplay.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
}

/**
 * Скрытие сообщений об ошибках
 */
function hideErrors() {
    errorDisplay.style.display = 'none';
    errorList.innerHTML = '';
}

/**
 * Установка состояния загрузки
 * @param {boolean} isLoading Идет ли загрузка
 */
function setLoadingState(isLoading) {
    calculateBtn.disabled = isLoading;
    resetBtn.disabled = isLoading;
    
    // Отключение всех полей ввода
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
 * Форматирование числа до четырех десятичных знаков
 * @param {number} value Значение
 * @returns {string} Отформатированная строка
 */
function formatNumber(value) {
    if (typeof value !== 'number' || isNaN(value)) {
        return 'N/A';
    }
    return value.toFixed(4);
}

/**
 * Генерация тестовых данных визуализации (для тестирования, реальные данные будут получены из API)
 * @param {Object} formData Данные формы
 * @returns {Object} Объект данных визуализации
 */
function generateMockVisualizationData(formData) {
    const spotPrice = formData.spot_price;
    const strikePrice = formData.strike_price;
    const volatility = formData.volatility;
    const timeToMaturity = formData.time_to_maturity;
    const riskFreeRate = formData.risk_free_rate;
    const isCall = formData.option_type === 'call';
    
    // Генерация диапазона цен базового актива (±40% от текущей цены)
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
    
    // Упрощенный расчет Блэка-Шоулза (для генерации тестовых данных)
    for (let i = 0; i < numPoints; i++) {
        const S = spotMin + i * spotStep;
        priceX.push(S);
        
        // Упрощенный расчет цены опциона
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
        
        // Упрощенный расчет греков
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
        
        // Theta (упрощенно)
        const theta = -(S * normPdf * volatility) / (2 * Math.sqrt(timeToMaturity)) - 
                      riskFreeRate * strikePrice * Math.exp(-riskFreeRate * timeToMaturity) * (isCall ? normCdf : (normCdf - 1));
        thetaY.push(theta / 365); // Преобразование в дневное значение
        
        // Vega
        const vega = S * normPdf * Math.sqrt(timeToMaturity);
        vegaY.push(vega / 100); // Преобразование для изменения волатильности на 1%
        
        // Rho
        const rho = strikePrice * timeToMaturity * Math.exp(-riskFreeRate * timeToMaturity) * (isCall ? normCdf : (normCdf - 1));
        rhoY.push(rho / 100); // Преобразование для изменения ставки на 1%
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
 * Отображение результатов расчета
 * @param {Object} result Объект результатов расчета
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
    
    // Обновление содержимого и отображение области результатов
    resultsContent.innerHTML = html;
    resultsSection.style.display = 'block';
    
    // Плавная прокрутка к области результатов
    resultsSection.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

/**
 * Скрытие отображения результатов
 */
function hideResults() {
    const resultsSection = document.getElementById('resultsSection');
    resultsSection.style.display = 'none';
    hideVisualization();
}

// ============================================================================
// Функции визуализации графиков
// ============================================================================

// Хранение экземпляров графиков
let priceChartInstance = null;
let greeksChartInstance = null;

/**
 * Отображение графиков визуализации
 * @param {Object} visualizationData Объект данных визуализации
 */
function displayVisualization(visualizationData) {
    if (!visualizationData) {
        return;
    }
    
    const visualizationSection = document.getElementById('visualizationSection');
    
    // Отображение области визуализации
    visualizationSection.style.display = 'block';
    
    // Рендеринг графика кривой цены
    if (visualizationData.price_curve) {
        renderPriceChart(visualizationData.price_curve);
    }
    
    // Рендеринг графика кривых греков
    if (visualizationData.greeks_curves) {
        renderGreeksChart(visualizationData.greeks_curves);
    }
    
    // Плавная прокрутка к области визуализации
    visualizationSection.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

/**
 * Скрытие графиков визуализации
 */
function hideVisualization() {
    const visualizationSection = document.getElementById('visualizationSection');
    visualizationSection.style.display = 'none';
    
    // Уничтожение существующих экземпляров графиков
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
 * Рендеринг графика кривой цены
 * @param {Object} priceData Данные кривой цены {x: [], y: []}
 */
function renderPriceChart(priceData) {
    const ctx = document.getElementById('priceChart');
    
    // Уничтожение существующего графика
    if (priceChartInstance) {
        priceChartInstance.destroy();
    }
    
    // Создание нового графика
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
 * Рендеринг графика кривых греков (несколько кривых)
 * @param {Object} greeksData Данные греков {delta: {x: [], y: []}, gamma: {x: [], y: []}, ...}
 */
function renderGreeksChart(greeksData) {
    const ctx = document.getElementById('greeksChart');
    
    // Уничтожение существующего графика
    if (greeksChartInstance) {
        greeksChartInstance.destroy();
    }
    
    // Определение цветов и меток греков
    const greeksConfig = {
        delta: { label: 'Delta (Δ)', color: '#667eea', borderWidth: 3 },
        gamma: { label: 'Gamma (Γ)', color: '#f093fb', borderWidth: 3 },
        theta: { label: 'Theta (Θ)', color: '#4facfe', borderWidth: 3 },
        vega: { label: 'Vega (ν)', color: '#43e97b', borderWidth: 3 },
        rho: { label: 'Rho (ρ)', color: '#fa709a', borderWidth: 3 }
    };
    
    // Построение наборов данных
    const datasets = [];
    for (const [greekName, greekData] of Object.entries(greeksData)) {
        if (greeksConfig[greekName] && greekData && greekData.x && greekData.y) {
            const config = greeksConfig[greekName];
            datasets.push({
                label: config.label,
                data: greekData.y,
                borderColor: config.color,
                backgroundColor: config.color + '20', // Добавление прозрачности
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
    
    // Использование данных оси x первого грека в качестве меток
    const xLabels = greeksData.delta ? greeksData.delta.x : 
                    greeksData.gamma ? greeksData.gamma.x :
                    greeksData.theta ? greeksData.theta.x :
                    greeksData.vega ? greeksData.vega.x :
                    greeksData.rho ? greeksData.rho.x : [];
    
    // Создание нового графика
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

// Экспорт функций для использования другими модулями (если необходимо)
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
