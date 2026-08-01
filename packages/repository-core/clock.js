export class ManualRepositoryClock {
  constructor(initial = 0) { this.value = initial; }
  now() { return this.value; }
  advance(milliseconds) { this.value += milliseconds; }
  set(value) { this.value = value; }
}
