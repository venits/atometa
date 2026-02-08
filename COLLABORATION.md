# GitHub Collaboration Guide

Complete guide for adding collaborators and working together on Atometa.

## Quick Start - Adding Collaborators

### Step 1: Push Project to GitHub

If not already done:

```cmd
cd C:\atometa
git init
git add .
git commit -m "Initial commit: Atometa v0.1.0"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/atometa.git
git push -u origin main
```

### Step 2: Add Collaborators

**On GitHub Website:**

1. Go to your repository: `https://github.com/YOUR_USERNAME/atometa`
2. Click **Settings** tab
3. Click **Collaborators** in left sidebar
4. Click **Add people** button
5. Enter collaborator's GitHub username or email
6. Select permission level
7. Click **Add [username] to this repository**

**Collaborator will receive email invitation**

### Step 3: Collaborator Accepts Invitation

1. Collaborator checks email
2. Clicks **View invitation** link
3. Clicks **Accept invitation** button
4. Now has access to repository

### Step 4: Collaborator Clones Repository

```cmd
git clone https://github.com/YOUR_USERNAME/atometa.git
cd atometa
```

## Permission Levels

### Read
- View code
- Clone repository
- Download releases
- Open issues
- Comment on pull requests

### Triage
- Read access PLUS:
- Manage issues and pull requests
- Apply labels
- Close/reopen issues

### Write
- Triage access PLUS:
- Push to repository
- Create branches
- Merge pull requests
- Close pull requests

### Maintain
- Write access PLUS:
- Manage repository settings
- Manage team access

### Admin
- Full access to everything
- Delete repository
- Transfer ownership
- Manage security settings

**Recommended for Atometa:**
- PhD supervisor: **Admin** (full oversight)
- Team members: **Write** (can push code)
- Reviewers: **Triage** (can review, no push)
- External contributors: **Read** (fork and PR)

## Collaborative Workflow

### Branch Strategy

```
main                    Stable, production-ready code
├── develop            Active development
│   ├── feature/renderer       Individual features
│   ├── feature/chemistry
│   ├── feature/ui
│   └── hotfix/critical-bug    Urgent fixes
```

### Team Workflow

**1. Never Push Directly to Main**

```cmd
# DON'T DO THIS
git checkout main
git commit -m "changes"
git push

# DO THIS INSTEAD
git checkout -b feature/my-feature
git commit -m "Add my feature"
git push origin feature/my-feature
# Then create Pull Request on GitHub
```

**2. Create Feature Branches**

```cmd
# Start new feature
git checkout develop
git pull origin develop
git checkout -b feature/add-bonds

# Work on feature
# ... make changes ...
git add .
git commit -m "feat: Add bond rendering system"

# Push to GitHub
git push origin feature/add-bonds
```

**3. Create Pull Request**

On GitHub:
1. Go to repository
2. Click **Pull requests** tab
3. Click **New pull request**
4. Base: `develop` ← Compare: `feature/add-bonds`
5. Click **Create pull request**
6. Add description of changes
7. Request reviewers
8. Assign to yourself
9. Add labels (enhancement, bug, documentation)
10. Click **Create pull request**

**4. Code Review Process**

Reviewer:
1. Reviews code changes
2. Adds comments on specific lines
3. Requests changes OR approves
4. Optional: Runs code locally to test

Author:
1. Addresses review comments
2. Pushes additional commits
3. Responds to comments
4. Requests re-review

**5. Merge Pull Request**

After approval:
1. Click **Merge pull request**
2. Choose merge strategy:
   - **Merge commit** (preserves history)
   - **Squash and merge** (clean history)
   - **Rebase and merge** (linear history)
3. Click **Confirm merge**
4. Delete branch (optional but recommended)

## Team Setup

### Create GitHub Team

1. Go to organization (if using one)
2. Click **Teams** tab
3. Click **New team**
4. Enter team name: "Atometa Developers"
5. Add description
6. Set visibility: **Visible** or **Secret**
7. Click **Create team**
8. Add members to team

### Assign Team to Repository

1. Repository **Settings** → **Collaborators and teams**
2. Click **Add teams**
3. Select "Atometa Developers"
4. Choose permission level: **Write**
5. Click **Add team**

## Communication Setup

### 1. GitHub Issues

**Create Issue Templates:**

Create `.github/ISSUE_TEMPLATE/bug_report.md`:

```markdown
---
name: Bug Report
about: Report a bug in Atometa
title: '[BUG] '
labels: bug
assignees: ''
---

## Bug Description
A clear description of the bug.

## Steps to Reproduce
1. Go to '...'
2. Click on '...'
3. See error

## Expected Behavior
What should happen.

## Actual Behavior
What actually happens.

## Environment
- OS: Windows 10
- Compiler: MSVC 2022
- Build: Release/Debug

## Screenshots
If applicable.

## Additional Context
Any other information.
```

Create `.github/ISSUE_TEMPLATE/feature_request.md`:

```markdown
---
name: Feature Request
about: Suggest a feature for Atometa
title: '[FEATURE] '
labels: enhancement
assignees: ''
---

## Feature Description
Clear description of the feature.

## Use Case
Why is this feature needed?

## Proposed Solution
How should this work?

## Alternatives Considered
Other approaches you've thought about.

## Additional Context
Mockups, diagrams, etc.
```

### 2. GitHub Discussions

Enable Discussions for Q&A and general chat:

1. Repository **Settings**
2. Scroll to **Features**
3. Check **Discussions**
4. Click **Set up discussions**

Categories:
- **Announcements** - Project updates
- **General** - General discussion
- **Ideas** - Feature brainstorming
- **Q&A** - Questions and answers
- **Show and tell** - Share work

### 3. GitHub Projects

Create project board for task management:

1. Repository → **Projects** tab
2. Click **New project**
3. Choose template:
   - **Board** (Kanban style)
   - **Table** (Spreadsheet style)
4. Name: "Atometa Development"

Columns:
- **Backlog** - Future tasks
- **To Do** - Planned for current sprint
- **In Progress** - Currently working
- **Review** - Awaiting code review
- **Done** - Completed

### 4. External Communication

**Recommended Tools:**

- **Discord/Slack** - Real-time chat
- **Weekly meetings** - Video calls
- **Shared calendar** - Deadlines, meetings
- **Documentation** - Notion, Google Docs

## Development Rules

### Code Standards

**Create CONTRIBUTING.md:**

```markdown
# Contributing to Atometa

## Code Style

- Use C++17 features
- Follow Google C++ Style Guide
- Use meaningful variable names
- Add comments for complex logic
- Maximum line length: 100 characters

## Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

Types:
- feat: New feature
- fix: Bug fix
- docs: Documentation
- style: Formatting
- refactor: Code restructure
- test: Add tests
- chore: Maintenance

Example:
```
feat: Add cylinder mesh generation

Implement CreateCylinder method in Mesh class.
Supports configurable radius, height, and sectors.

Closes #42
```

## Pull Request Process

1. Create feature branch
2. Make changes
3. Add tests
4. Update documentation
5. Push branch
6. Create PR
7. Request review
8. Address feedback
9. Merge when approved

## Testing Requirements

- All new features must have tests
- Maintain 80%+ code coverage
- All tests must pass before merge

## Review Checklist

- [ ] Code builds without warnings
- [ ] Tests pass
- [ ] Documentation updated
- [ ] No merge conflicts
- [ ] Follows code style
```

### Protect Main Branch

**Settings → Branches → Add rule:**

1. Branch name pattern: `main`
2. Check these options:
   - ✅ Require a pull request before merging
   - ✅ Require approvals (1-2 reviewers)
   - ✅ Dismiss stale reviews
   - ✅ Require review from Code Owners
   - ✅ Require status checks to pass
   - ✅ Require branches to be up to date
   - ✅ Include administrators
3. Click **Create**

### Protect Develop Branch

Same as main, but allow more flexibility:
- Require 1 approval (instead of 2)
- Allow force pushes from admins

## Daily Workflow

### Morning

```cmd
# Update your local repository
git checkout develop
git pull origin develop

# Create/continue feature branch
git checkout feature/my-feature
git merge develop  # Bring in latest changes

# Start coding
```

### During Day

```cmd
# Commit frequently (every 30-60 minutes)
git add .
git commit -m "feat: Add atom color picker"

# Push to GitHub (backup and visibility)
git push origin feature/my-feature
```

### End of Day

```cmd
# Final commit
git add .
git commit -m "wip: Bond rendering in progress"
git push origin feature/my-feature

# Optional: Create draft PR for early feedback
```

## Conflict Resolution

### When Conflicts Occur

```cmd
# Update your branch
git checkout feature/my-feature
git fetch origin
git merge origin/develop

# If conflicts appear
# CONFLICT (content): Merge conflict in src/core/Application.cpp

# Open conflicted file, look for:
<<<<<<< HEAD
Your changes
=======
Their changes
>>>>>>> origin/develop

# Resolve manually, then:
git add src/core/Application.cpp
git commit -m "merge: Resolve conflicts with develop"
git push origin feature/my-feature
```

### Preventing Conflicts

1. Pull from develop frequently
2. Communicate about which files you're working on
3. Keep feature branches small and short-lived
4. Use separate files when possible

## Code Review Best Practices

### For Reviewers

**Good Review Comments:**
```
✅ "Consider extracting this logic into a separate function for reusability."
✅ "This could cause a null pointer exception if atom is nullptr."
✅ "Great job handling edge cases!"
✅ "Have you considered using a switch statement here?"
```

**Bad Review Comments:**
```
❌ "This is wrong."
❌ "Why did you do it this way?"
❌ "I don't like this."
```

**Review Checklist:**
- [ ] Code is readable and well-structured
- [ ] No obvious bugs or logic errors
- [ ] Edge cases handled
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] Performance considerations
- [ ] Security issues (if applicable)
- [ ] Consistent with project style

### For Authors

**Responding to Reviews:**
```
✅ "Good catch! Fixed in commit abc123."
✅ "I chose this approach because... What do you think?"
✅ "I'll create a follow-up issue for that enhancement."

❌ "That's not a bug."
❌ "It works fine."
❌ "You don't understand."
```

## Team Meetings

### Weekly Sprint Planning

**Agenda:**
1. Review last week's progress
2. Discuss blockers and challenges
3. Plan this week's tasks
4. Assign issues to team members
5. Set sprint goals

**Duration:** 30-60 minutes

### Daily Standups (Optional)

**Format (15 minutes max):**
- What did you do yesterday?
- What will you do today?
- Any blockers?

**Can be async via Discord/Slack**

### Monthly Review

**Agenda:**
1. Demo completed features
2. Review metrics (code coverage, bug count)
3. Discuss technical debt
4. Update roadmap
5. Celebrate achievements!

## Repository Setup Checklist

- [ ] Repository created on GitHub
- [ ] README.md with project description
- [ ] LICENSE file (MIT)
- [ ] .gitignore configured
- [ ] CONTRIBUTING.md with guidelines
- [ ] Issue templates created
- [ ] Pull request template created
- [ ] Branch protection rules set
- [ ] Collaborators added
- [ ] GitHub Actions for CI/CD (optional)
- [ ] Project board created
- [ ] Discussions enabled
- [ ] Wiki initialized (optional)

## Access Management

### Audit Access Regularly

**Every 3 months:**
1. Review collaborator list
2. Remove inactive members
3. Update permissions as needed
4. Check team memberships

### Onboarding New Members

**Checklist for new collaborators:**
1. Add to repository
2. Add to team (if using teams)
3. Send welcome message with:
   - Link to CONTRIBUTING.md
   - Link to development setup guide
   - Slack/Discord invite
   - Assign "good first issue"
4. Schedule onboarding call

### Offboarding

**When someone leaves:**
1. Remove from repository
2. Remove from teams
3. Revoke external tool access
4. Transfer assigned issues
5. Archive their branches (don't delete)

## Example Team Structure

### For PhD Project

```
Repository Owner (You)
├── Supervisor (Admin)
├── Co-supervisor (Write)
├── Research Team (Write)
│   ├── Team Member 1
│   ├── Team Member 2
│   └── Team Member 3
├── Reviewers (Triage)
│   ├── Chemistry Expert
│   └── Computer Science Expert
└── External Contributors (Read → Fork + PR)
```

### Responsibilities

**You (Owner):**
- Overall project direction
- Final decisions on architecture
- Merge to main branch
- Release management

**Supervisor:**
- Code review
- Architecture guidance
- Research direction

**Team Members:**
- Feature development
- Bug fixes
- Testing
- Documentation

**Reviewers:**
- Domain expertise
- Code quality review
- Research validation

## Security Best Practices

### Sensitive Data

**NEVER commit:**
- API keys
- Passwords
- Database credentials
- Private keys
- Personal data

**Use .gitignore:**
```
# Add to .gitignore
*.key
*.pem
secrets.json
.env
config/local.json
```

**Use GitHub Secrets for CI/CD:**
Settings → Secrets and variables → Actions

### Security Scanning

Enable Dependabot:
1. Settings → Security → Code security and analysis
2. Enable Dependabot alerts
3. Enable Dependabot security updates

## Troubleshooting

### Common Issues

**Issue: Collaborator can't push**

Solution:
1. Check they've accepted invitation
2. Verify permission level (needs Write or higher)
3. Check branch protection rules

**Issue: Can't see someone's branch**

Solution:
```cmd
git fetch origin
git checkout origin/their-branch
```

**Issue: Merge conflicts**

Solution: See "Conflict Resolution" section above

**Issue: Accidentally pushed to main**

Solution:
```cmd
# If you haven't pushed yet
git reset HEAD~1

# If you've already pushed (requires admin)
# Contact repository admin to revert
```

## Resources

- GitHub Docs: https://docs.github.com
- Git Book: https://git-scm.com/book
- GitHub Skills: https://skills.github.com
- Git Cheat Sheet: https://education.github.com/git-cheat-sheet-education.pdf

---

Last Updated: February 2026