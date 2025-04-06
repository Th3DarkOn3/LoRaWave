import { Component, EnvironmentInjector, inject } from '@angular/core';
import { IonTabs, IonTabBar, IonTabButton, IonIcon, IonLabel, IonTitle, IonToolbar, IonHeader, IonContent } from '@ionic/angular/standalone';
import { addIcons } from 'ionicons';
import { homeOutline, mapOutline, calendarOutline, calendarNumberOutline } from 'ionicons/icons';

@Component({
  selector: 'app-tabs',
  templateUrl: 'tabs.page.html',
  styleUrls: ['tabs.page.scss'],
  standalone: true,
  imports: [IonTabs, IonTabBar, IonTabButton, IonIcon, IonLabel, IonTitle, IonToolbar, IonHeader, IonContent],})
export class TabsPage {
  public environmentInjector = inject(EnvironmentInjector);

  constructor() {
    addIcons({ homeOutline, mapOutline, calendarOutline, calendarNumberOutline });
  }
}
