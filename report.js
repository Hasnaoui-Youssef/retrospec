const fs = require('fs');

// Parse roles and their relationships from .gv file
function parseGraph(gvContent) {
    const edges = [];
    const roles = new Set();
    const lines = gvContent.split('\n');

    lines.forEach(line => {
        if (line.includes('->')) {
            const [source, target] = line
                .split('->')
                .map(s => s.trim().replace(/[";]/g, '').split(/\[/)[0]);
            edges.push({ source, target });
            roles.add(source).add(target);
        }
    });

    return { roles: Array.from(roles), edges };
}

// Build hierarchical relationships
function buildHierarchy(edges) {
    const childrenMap = new Map();  // parent -> [children]
    const parentsMap = new Map();   // child -> [parents]

    edges.forEach(({ source, target }) => {
        if (!childrenMap.has(target)) childrenMap.set(target, []);
        childrenMap.get(target).push(source);

        if (!parentsMap.has(source)) parentsMap.set(source, []);
        parentsMap.get(source).push(target);
    });

    return { childrenMap, parentsMap };
}

// Get all descendants recursively
function getDescendants(role, childrenMap) {
    const descendants = new Set();
    const stack = [role];

    while (stack.length > 0) {
        const current = stack.pop();
        const children = childrenMap.get(current) || [];

        for (const child of children) {
            if (!descendants.has(child)) {
                descendants.add(child);
                stack.push(child);
            }
        }
    }
    return Array.from(descendants);
}

// Generate random grants
function generateGrants(role) {
    const grants = [];
    const numGrants = Math.floor(Math.random() * 3); // 0-2 grants

    const databases = ['finance', 'hr', 'dev', 'erp', 'crm'];
    const tables = {
        finance: ['transactions', 'accounts'],
        hr: ['employees', 'salaries'],
        dev: ['projects', 'tasks'],
        erp: ['orders', 'inventory'],
        crm: ['clients', 'contacts']
    };

    const privileges = ['SELECT', 'INSERT', 'UPDATE', 'DELETE'];
    const columns = ['id', 'name', 'amount', 'date'];

    for (let i = 0; i < numGrants; i++) {
        const db = databases[Math.floor(Math.random() * databases.length)];
        grants.push({
            type: Math.random() < 0.75 ? 'GRANT' : 'REVOKE',
            privs: [
              {
                name: privileges[Math.floor(Math.random() * privileges.length)],
                cols: Math.random() < 0.6 ? ['ALL'] :
                    Array.from({length: 2}, () =>
                        columns[Math.floor(Math.random() * columns.length)])
              }
            ],
            database: db,
            table: tables[db][Math.floor(Math.random() * tables[db].length)],
            user: role
        });
    }
    return grants;
}

// Aggregate grants from descendants
function aggregateGrants(role, baseGrants, childrenMap) {
    const allGrants = [...baseGrants.get(role)];
    const descendants = getDescendants(role, childrenMap);

    descendants.forEach(child => {
        const childGrants = baseGrants.get(child) || [];
        allGrants.push(...childGrants);
    });

    return allGrants;
}

// Main function
function generateReport(gvFile, outputFile = 'report.json') {
    const gvContent = fs.readFileSync(gvFile, 'utf8');
    const { roles, edges } = parseGraph(gvContent);
    const { childrenMap } = buildHierarchy(edges);

    // Generate base grants
    const baseGrants = new Map();
    roles.forEach(role => {
        baseGrants.set(role, generateGrants(role));
    });

    // Build final report with aggregated grants
    const report = {
        DBMS: "MySQL",
        server: "0.0.0.0",
        connectedUser: "root",
        port: "3306",
        userGrants: {}
    };

    roles.forEach(role => {
        report.userGrants[role] = aggregateGrants(role, baseGrants, childrenMap);
    });

    fs.writeFileSync(outputFile, JSON.stringify(report, null, 2));
    console.log(`Report generated: ${outputFile}`);
}

// Run
generateReport('test.gv');
