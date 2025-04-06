import { Component } from '@angular/core';
import { provideHttpClient } from '@angular/common/http';
import { bootstrapApplication } from '@angular/platform-browser';
import { provideRouter } from '@angular/router';
import { IonApp, IonRouterOutlet, provideIonicAngular } from '@ionic/angular/standalone';
import { AngularDelegate } from '@ionic/angular';

import { routes } from './app.routes';

@Component({
  selector: 'app-root',
  templateUrl: 'app.component.html',
  standalone: true,
  imports: [IonApp, IonRouterOutlet],
})
export class AppComponent {
  constructor() {}
}

bootstrapApplication(AppComponent, {
  providers: [provideHttpClient(), provideRouter(routes), AngularDelegate, provideIonicAngular()],
});
