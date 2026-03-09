# GitHub Setup Guide

Complete guide for uploading Atometa to GitHub.

## Step 1: Create GitHub Repository

1. Go to https://github.com and log in
2. Click the "+" button in top right corner
3. Click "New repository"
4. Fill in repository details:
   - Repository name: `atometa`
   - Description: `3D Chemistry Simulation Engine for Research and Education`
   - Visibility: Public (or Private if preferred)
   - DO NOT initialize with README (we already have one)
   - DO NOT add .gitignore (we already have one)
   - DO NOT add license (we already have MIT)
5. Click "Create repository"

## Step 2: Initialize Git Repository

Open Git Bash or Command Prompt in C:\atometa:

```bash
git init
```

## Step 3: Add All Files

```bash
git add .
```

## Step 4: Create Initial Commit

```bash
git commit -m "Initial commit: Atometa v0.1.0 - Project structure and core framework"
```

## Step 5: Add Remote Repository

Replace `yourusername` with your actual GitHub username:

```bash
git remote add origin https://github.com/yourusername/atometa.git
```

## Step 6: Push to GitHub

```bash
git branch -M main
git push -u origin main
```

Enter your GitHub credentials when prompted.

## Step 7: Configure Repository Settings

On GitHub, go to your repository page:

### Add Topics

Click the gear icon next to "About" and add:
- chemistry
- 3d-visualization
- molecular-dynamics
- cpp
- opengl
- cmake
- phd-research
- education
- scientific-computing

### Update Description

Set description:
```
3D Chemistry Simulation Engine - PhD Research Project for molecular visualization and dynamics
```

### Enable Issues

Settings -> Features -> Check "Issues"

## Git Workflow

### Daily Commits

```bash
# Check what changed
git status

# Add all changes
git add .

# Commit with descriptive message
git commit -m "feat: Add Camera class with orbital controls"

# Push to GitHub
git push
```

### Commit Message Format

Use conventional commit format:

```
feat: Add new feature
fix: Fix bug
docs: Update documentation
refactor: Code refactoring
test: Add tests
perf: Performance improvement
```

Examples:
```bash
git commit -m "feat: Implement Shader class with GLSL compilation"
git commit -m "fix: Fix memory leak in Atom destructor"
git commit -m "docs: Update README with build instructions"
git commit -m "refactor: Improve Window initialization logic"
```

### Branching Strategy

```bash
# Create feature branch
git checkout -b feature/renderer-pipeline

# Work on feature...
git add .
git commit -m "feat: Add basic rendering pipeline"

# Push feature branch
git push origin feature/renderer-pipeline

# Switch back to main
git checkout main

# Merge feature (when ready)
git merge feature/renderer-pipeline
git push origin main
```

## What Files Will Be Uploaded

Files that WILL be uploaded:
```
src/                  All source code
include/              All headers
assets/branding/      Logo and branding
docs/                 Documentation
CMakeLists.txt        Build configuration
vcpkg.json            Dependencies
README.md             Main documentation
BUILD_GUIDE.md        Build instructions
LIBRARIES.md          Library reference
ROADMAP.md            Development plan
LICENSE               MIT License
.gitignore            Git ignore rules
```

Files that WILL NOT be uploaded (in .gitignore):
```
build/                Build outputs
.vs/                  Visual Studio files
*.exe                 Executables
*.obj                 Object files
*.pdb                 Debug files
vcpkg_installed/      vcpkg cache
```

## Repository Structure on GitHub

Your repository will look like:

```
yourusername/atometa
├── README.md
├── LICENSE
├── CMakeLists.txt
├── vcpkg.json
├── .gitignore
├── src/
├── include/
├── assets/
└── docs/
```

## After First Push

### Add Repository Image

1. Go to Settings -> General
2. Under "Social preview" click "Edit"
3. Upload the logo (assets/branding/logo.png)

### Create First Release (Optional)

After MVP is complete:

1. Go to Releases -> "Create a new release"
2. Tag: `v0.1.0`
3. Title: `Atometa v0.1.0 - MVP Release`
4. Description: Describe what's included
5. Attach compiled executable if desired

### Enable GitHub Pages (Optional)

For documentation hosting:

1. Settings -> Pages
2. Source: Deploy from branch
3. Branch: main
4. Folder: /docs
5. Save

## Common Git Commands

```bash
# View commit history
git log --oneline

# View changes
git diff

# Undo last commit (keep changes)
git reset --soft HEAD~1

# Undo last commit (discard changes)
git reset --hard HEAD~1

# View remote URL
git remote -v

# Pull latest changes
git pull

# Clone repository to another machine
git clone https://github.com/yourusername/atometa.git
```

## GitHub Features to Use

### Issues

Create issues for:
- Bugs
- Feature requests
- TODO items

Example:
```
Title: Implement Camera orbital controls
Labels: enhancement, renderer
Milestone: MVP v0.1.0
Description: Add camera rotation, zoom, and pan controls
```

### Projects

Create a project board:
1. Projects -> New project
2. Choose "Board" template
3. Columns: To Do, In Progress, Testing, Done
4. Add issues as cards

### Wiki

Enable wiki for:
- Extended documentation
- Architecture decisions
- Research notes
- Tutorials

### Discussions

Enable discussions for:
- Questions
- Ideas
- Announcements

## Protecting Your Work

### Important: DO NOT Upload

NEVER commit these to Git:
- API keys or passwords
- Personal data
- Large binary files (>100 MB)
- Compiled executables (use Releases instead)

### Using .gitignore

Already configured in your project. If you need to add more:

```bash
# Edit .gitignore
notepad .gitignore

# Add patterns
*.secret
private/
data/large_dataset.bin
```

## Collaborating

### Accepting Contributions

1. Contributors fork your repository
2. They create feature branch
3. They submit Pull Request
4. You review and merge

### Code Review Checklist

Before merging:
- Code compiles without errors
- No breaking changes
- Code follows project style
- Proper commit messages

## Backup Strategy

GitHub is your backup, but also:

1. Commit and push daily
2. Create branches before major changes
3. Tag important milestones
4. Keep local backups of research data

## Continuous Development

### Daily Workflow

```bash
# Morning - start work
git pull

# During day - commit frequently
git add .
git commit -m "feat: Implement X"

# Evening - push to GitHub
git push
```

### Weekly Workflow

```bash
# Monday - create weekly branch
git checkout -b week-5-renderer

# Throughout week - work and commit

# Friday - merge to main
git checkout main
git merge week-5-renderer
git push
```

## Getting Help

### Git Problems

```bash
# Forgot to pull before commit
git pull --rebase

# Merge conflict
# Edit conflicted files
git add .
git commit

# Push rejected
git pull
git push
```

### GitHub Problems

- Authentication issues: Use Personal Access Token instead of password
- Large files: Use Git LFS or don't commit them
- Slow uploads: Compress or remove large files from history

## Next Steps

After pushing to GitHub:

1. Star your own repository
2. Add topics and description
3. Create first issue: "Setup MVP roadmap"
4. Invite supervisors as collaborators (if desired)
5. Share repository link in thesis/papers

---

Your code is now safely backed up and ready for collaboration!