import './style.css'
import { readSimulatorParams } from './params'
import { buildWebUrl } from './repo-provider'
import { loadSkill } from './skill-loader'
import { RuntimeHost } from './runtime-host'
import type { LoadedSkill } from './types'

const app = document.querySelector<HTMLDivElement>('#app')
if (!app) throw new Error('missing #app')
const appRoot = app

let loadedSkill: LoadedSkill | null = null
let runtime: RuntimeHost | null = null

function escapeHtml(value: string): string {
  return value
    .replaceAll('&', '&amp;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;')
    .replaceAll('"', '&quot;')
    .replaceAll("'", '&#39;')
}

function renderShell(): HTMLIFrameElement {
  appRoot.innerHTML = `
    <div class="app">
      <header class="topbar">
        <div class="title">
          <h1>ESP-Claw Simulator</h1>
          <p id="subtitle">Loading Skill...</p>
        </div>
        <div class="actions">
          <button id="run" class="button primary" type="button" disabled>Run</button>
          <button id="stop" class="button" type="button" disabled>Stop</button>
        </div>
      </header>
      <div class="layout">
        <aside class="panel">
          <h2>Skill</h2>
          <div id="meta" class="meta"></div>
          <div id="status" class="status">Preparing...</div>
        </aside>
        <section class="runtime">
          <iframe id="runtime" title="ESP-Claw Lua LVGL runtime"></iframe>
        </section>
      </div>
    </div>
  `

  return document.querySelector<HTMLIFrameElement>('#runtime')!
}

function setStatus(message: string, error = false): void {
  const el = document.querySelector<HTMLDivElement>('#status')
  if (!el) return
  el.textContent = message
  el.classList.toggle('error', error)
}

function renderSkill(skill: LoadedSkill): void {
  const subtitle = document.querySelector<HTMLParagraphElement>('#subtitle')
  if (subtitle) subtitle.textContent = skill.frontmatter.description

  const meta = document.querySelector<HTMLDivElement>('#meta')
  if (!meta) return

  const categories = skill.frontmatter.metadata.category ?? []
  const files = skill.files.map((file) => file.path)
  meta.innerHTML = `
    <div class="row">
      <span class="label">Name</span>
      <span class="value">${escapeHtml(skill.frontmatter.name)}</span>
    </div>
    <div class="row">
      <span class="label">Entry</span>
      <span class="value">${escapeHtml(skill.entry)}</span>
    </div>
    <div class="row">
      <span class="label">Source</span>
      <a class="value" href="${escapeHtml(buildWebUrl(skill.params, skill.params.skill))}" target="_blank" rel="noreferrer">SKILL.md</a>
    </div>
    <div class="row">
      <span class="label">Categories</span>
      <span class="tag-list">${categories.map((cat) => `<span class="tag">${escapeHtml(cat)}</span>`).join('')}</span>
    </div>
    <div class="row">
      <span class="label">Mounted files</span>
      <span class="value">${escapeHtml(files.join('\\n'))}</span>
    </div>
  `
}

async function main(): Promise<void> {
  const iframe = renderShell()
  runtime = new RuntimeHost(iframe)

  const runButton = document.querySelector<HTMLButtonElement>('#run')
  const stopButton = document.querySelector<HTMLButtonElement>('#stop')
  runButton?.addEventListener('click', () => {
    if (loadedSkill) {
      runtime?.run(loadedSkill)
      stopButton?.removeAttribute('disabled')
    }
  })
  stopButton?.addEventListener('click', () => runtime?.stop())

  try {
    await runtime.load()
    const params = readSimulatorParams()
    loadedSkill = await loadSkill(params)
    renderSkill(loadedSkill)
    setStatus('Skill loaded. Click Run to start the simulator.')
    runButton?.removeAttribute('disabled')
  } catch (error) {
    setStatus(error instanceof Error ? error.message : String(error), true)
  }
}

main()
