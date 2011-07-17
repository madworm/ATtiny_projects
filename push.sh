#!/bin/bash
git push --all gso && git push --all github && git push --all gitorious
git push --tags gso && git push --tags github && git push --tags gitorious

