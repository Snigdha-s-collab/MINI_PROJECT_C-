// App State
let appState = {
    user: JSON.parse(localStorage.getItem('user')) || null, // {id, type, name, token}
    internships: [],
    dashboard: null
};

// Main DOM Content Loaded
document.addEventListener('DOMContentLoaded', () => {
    initApp();
});

function toggleLoader(show) {
    if(show) document.getElementById('loader').classList.remove('hidden');
    else document.getElementById('loader').classList.add('hidden');
}

function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    toast.innerHTML = `<span>${message}</span>`;
    container.appendChild(toast);
    setTimeout(() => {
        toast.style.opacity = '0';
        setTimeout(() => toast.remove(), 300);
    }, 4000);
}

// Routes Definition
const routes = {
    '/': renderLanding,
    '/dashboard': renderDashboard,
    '/internships': renderInternships
};

function navigate(path) {
    if (path !== '/' && !appState.user) {
        path = '/';
        showToast('Please login first', 'error');
    } else if (path === '/' && appState.user) {
        path = '/dashboard';
    }
    
    // Update active nav links
    renderNav();
    window.location.hash = path;
    const content = document.getElementById('app-content');
    content.innerHTML = ''; // clear
    content.classList.remove('fade-in');
    void content.offsetWidth; // trigger reflow
    content.classList.add('fade-in');
    
    routes[path]();
}

// Handle Hash Routing
window.addEventListener('hashchange', () => {
    let hash = window.location.hash.replace('#', '') || '/';
    navigate(hash);
});

async function apiCall(endpoint, method = 'GET', body = null) {
    toggleLoader(true);
    try {
        const headers = { 'Content-Type': 'application/json' };
        if (appState.user) headers['Authorization'] = `Bearer ${appState.user.id}`; // Simple auth for demo
        
        const res = await fetch(endpoint, {
            method,
            headers,
            body: body ? JSON.stringify(body) : null
        });
        
        const data = await res.json();
        toggleLoader(false);
        if(!data.success) throw new Error(data.message || 'API Error');
        return data.data || data;
    } catch(err) {
        toggleLoader(false);
        showToast(err.message, 'error');
        throw err;
    }
}

function initApp() {
    let hash = window.location.hash.replace('#', '') || '/';
    navigate(hash);
}

function renderNav() {
    const navLinks = document.getElementById('nav-links');
    if(!appState.user) {
        navLinks.innerHTML = `
            <button class="btn btn-primary" onclick="navigate('/')">Login / Register</button>
        `;
    } else {
        navLinks.innerHTML = `
            <button class="btn btn-outline" style="border:none" onclick="navigate('/dashboard')">Dashboard</button>
            <button class="btn btn-outline" style="border:none" onclick="navigate('/internships')">Browse Internships</button>
            <button class="btn btn-outline" onclick="logout()">Logout</button>
        `;
    }
}

function logout() {
    localStorage.removeItem('user');
    appState.user = null;
    showToast('Logged out successfully');
    navigate('/');
}

// =======================
// VIEWS
// =======================

function renderLanding() {
    const content = document.getElementById('app-content');
    content.innerHTML = `
        <div class="auth-layout">
            <div class="auth-hero fade-in">
                <h1>Launch Your <br><span class="highlight">Career Today</span></h1>
                <p class="mt-2">Join the premium platform connecting ambitious students with top-tier companies. Your perfect internship awaits.</p>
                <div class="stats-grid mt-4">
                    <div class="stat-card" style="padding:1rem;">
                        <div class="stat-value" style="font-size:1.5rem">10k+</div>
                        <div class="stat-label">Students</div>
                    </div>
                    <div class="stat-card" style="padding:1rem;">
                        <div class="stat-value" style="font-size:1.5rem">500+</div>
                        <div class="stat-label">Companies</div>
                    </div>
                </div>
            </div>
            <div class="auth-form glass-panel fade-in" style="margin-top:0">
                <div class="auth-tabs">
                    <div class="auth-tab active" id="tab-login" onclick="switchAuthTab('login')">Sign In</div>
                    <div class="auth-tab" id="tab-register" onclick="switchAuthTab('register')">Register</div>
                </div>
                
                <div id="login-form">
                    <h3 class="mb-3">Welcome Back</h3>
                    <div class="form-group">
                        <label>Email Address</label>
                        <input type="email" id="login-email" class="form-control" placeholder="john@example.com">
                    </div>
                    <div class="form-group">
                        <label>Password</label>
                        <input type="password" id="login-pass" class="form-control" placeholder="••••••••">
                    </div>
                    <button class="btn btn-primary" style="width:100%" onclick="handleLogin()">Sign In</button>
                </div>
                
                <div id="register-form" class="hidden">
                    <h3 class="mb-3">Create Account</h3>
                    <div class="form-group mb-1">
                        <label>Full Name</label>
                        <input type="text" id="reg-name" class="form-control">
                    </div>
                    <div class="form-group mb-1">
                        <label>Email Address</label>
                        <input type="email" id="reg-email" class="form-control">
                    </div>
                    <div class="form-group mb-1">
                        <label>Password</label>
                        <input type="password" id="reg-pass" class="form-control">
                    </div>
                    <div class="form-group mb-1 flex gap-2">
                        <div style="flex:1">
                            <label>Current CGPA (0-10)</label>
                            <input type="number" id="reg-cgpa" class="form-control" step="0.1" max="10">
                        </div>
                    </div>
                    <div class="form-group mb-1">
                        <label>Skills (Comma separated)</label>
                        <input type="text" id="reg-skills" class="form-control" placeholder="C++, React, Node.js">
                    </div>
                    <div class="form-group mb-3">
                        <label>About You (Resume summary)</label>
                        <textarea id="reg-resume" class="form-control" rows="3"></textarea>
                    </div>
                    <button class="btn btn-primary" style="width:100%" onclick="handleRegister()">Create Student Account</button>
                </div>
            </div>
        </div>
    `;
}

function switchAuthTab(tab) {
    document.getElementById('tab-login').classList.remove('active');
    document.getElementById('tab-register').classList.remove('active');
    document.getElementById(`tab-${tab}`).classList.add('active');
    
    if(tab === 'login') {
        document.getElementById('login-form').classList.remove('hidden');
        document.getElementById('register-form').classList.add('hidden');
    } else {
        document.getElementById('login-form').classList.add('hidden');
        document.getElementById('register-form').classList.remove('hidden');
    }
}

async function handleLogin() {
    const email = document.getElementById('login-email').value;
    const pass = document.getElementById('login-pass').value;
    if(!email || !pass) return showToast('Please enter email and password', 'error');
    
    try {
        const data = await apiCall('/api/login/student', 'POST', { email, password: pass });
        appState.user = { id: data.id, name: data.name, type: 'student' };
        localStorage.setItem('user', JSON.stringify(appState.user));
        showToast(`Welcome back, ${data.name}!`);
        navigate('/dashboard');
    } catch(e) {}
}

async function handleRegister() {
    const data = {
        name: document.getElementById('reg-name').value,
        email: document.getElementById('reg-email').value,
        password: document.getElementById('reg-pass').value,
        cgpa: parseFloat(document.getElementById('reg-cgpa').value),
        skills: document.getElementById('reg-skills').value,
        resume: document.getElementById('reg-resume').value
    };
    
    if(!data.name || !data.email || !data.password) return showToast('Please fill required fields', 'error');
    if(data.password.length < 4) return showToast('Password minimum 4 chars', 'error');
    
    try {
        const res = await apiCall('/api/register/student', 'POST', data);
        showToast('Registration successful! You can now login.');
        switchAuthTab('login');
    } catch(e) {}
}

async function renderDashboard() {
    const content = document.getElementById('app-content');
    content.innerHTML = `<h2>Loading Dashboard...</h2>`;
    
    try {
        const dash = await apiCall('/api/dashboard/student');
        appState.dashboard = dash;
        
        let headerHTML = `
            <div class="mb-4">
                <h1>Hello, <span class="highlight">${appState.user.name}</span></h1>
                <p class="text-muted mt-2">Here is your internship applications overview.</p>
            </div>
            
            <div class="stats-grid">
                <div class="stat-card">
                    <div class="stat-value">${dash.totalApps}</div>
                    <div class="stat-label">Total Applications</div>
                </div>
                <div class="stat-card" style="border-bottom: 3px solid var(--primary)">
                    <div class="stat-value">${dash.applied}</div>
                    <div class="stat-label">In Review</div>
                </div>
                <div class="stat-card" style="border-bottom: 3px solid var(--success)">
                    <div class="stat-value">${dash.shortlisted}</div>
                    <div class="stat-label">Shortlisted</div>
                </div>
                <div class="stat-card" style="border-bottom: 3px solid #10b981">
                    <div class="stat-value">${dash.selected}</div>
                    <div class="stat-label">Selected</div>
                </div>
            </div>
            
            <h3 class="mb-3">My Applications</h3>
            <div class="card-grid" id="apps-grid">
        `;
        
        if(!dash.applications || dash.applications.length === 0) {
            headerHTML += `<div class="item-card"><p>You haven't applied to any internships yet.</p><button class="btn btn-primary mt-2" onclick="navigate('/internships')">Explore Internships</button></div>`;
        } else {
            dash.applications.forEach(app => {
                let badgeClass = 'badge-primary';
                if(app.status === 'SELECTED') badgeClass = 'badge-success';
                if(app.status === 'REJECTED') badgeClass = 'badge-danger';
                if(app.status === 'SHORTLISTED') badgeClass = 'badge-warning';
                
                headerHTML += `
                <div class="item-card">
                    <span class="badge ${badgeClass}">${app.status}</span>
                    <div class="item-title">${app.role}</div>
                    <div class="item-subtitle">${app.companyName}</div>
                    <div class="item-details">
                        <span><strong>Applied On:</strong> ${app.date}</span>
                        <span><strong>Resume Score:</strong> ${app.score}/100</span>
                        <span>
                            <strong>Match:</strong> ${app.match}%
                            <div class="progress-container"><div class="progress-bar" style="width: ${app.match}%"></div></div>
                        </span>
                    </div>
                </div>`;
            });
        }
        
        headerHTML += `</div>`;
        content.innerHTML = headerHTML;
    } catch(e) {
        content.innerHTML = `<h2>Failed to load dashboard.</h2><button class="btn btn-primary mt-2" onclick="navigate('/dashboard')">Retry</button>`;
    }
}

async function renderInternships() {
    const content = document.getElementById('app-content');
    content.innerHTML = `<h2>Loading Internships...</h2>`;
    
    try {
        const internships = await apiCall('/api/internships');
        appState.internships = internships;
        
        let html = `
            <div class="flex justify-between align-center mb-4">
                <div>
                    <h1>Open <span class="highlight">Internships</span></h1>
                    <p class="text-muted mt-2">Find your next big opportunity here.</p>
                </div>
            </div>
            <div class="card-grid">
        `;
        
        if(internships.length === 0) {
            html += `<div class="item-card"><p>No open internships at the moment.</p></div>`;
        }
        
        internships.forEach(i => {
            const skillsHtml = i.skills.map(s => `<span class="tag">${s}</span>`).join('');
            html += `
            <div class="item-card">
                <span class="badge badge-primary">Rs. ${i.stipend}/mo</span>
                <div class="item-title">${i.role}</div>
                <div class="item-subtitle">${i.companyName} • ${i.location}</div>
                <div class="item-details">
                    <span><strong>Duration:</strong> ${i.duration} months</span>
                    <span><strong>Min CGPA:</strong> ${i.minCGPA}</span>
                    <span><strong>Deadline:</strong> ${i.deadline}</span>
                    <div class="tags mt-2">${skillsHtml}</div>
                </div>
                <div class="item-actions">
                    <button class="btn btn-primary" style="width:100%" onclick="applyForInternship(${i.id}, '${i.role}')">Apply Now</button>
                </div>
            </div>`;
        });
        
        html += `</div>`;
        content.innerHTML = html;
        
    } catch(e) {
        content.innerHTML = `<h2>Failed to load internships.</h2>`;
    }
}

async function applyForInternship(internshipId, role) {
    if(!confirm(`Are you sure you want to apply for ${role}?`)) return;
    try {
        const result = await apiCall('/api/apply', 'POST', { internshipId });
        showToast(result.message || 'Application submitted perfectly!');
        navigate('/dashboard'); // Go to dash to see app
    } catch(e) {}
}
